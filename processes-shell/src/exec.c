// src/exec.c
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "shell.h"
#include "jobs.h"

/* Helper: find executable in PATH (simple) */
static char *which_in_path(const char *cmd)
{
    if (!cmd)
        return NULL;
    if (strchr(cmd, '/'))
        return strdup(cmd); /* contains slash -> direct path */
    char *path = getenv("PATH");
    if (!path)
        return NULL;
    char *pfx = strdup(path);
    char *saveptr = NULL;
    char *dir = strtok_r(pfx, ":", &saveptr);
    while (dir)
    {
        size_t len = strlen(dir) + 1 + strlen(cmd) + 1;
        char *candidate = malloc(len);
        if (!candidate)
        {
            dir = strtok_r(NULL, ":", &saveptr);
            continue;
        }
        snprintf(candidate, len, "%s/%s", dir, cmd);
        if (access(candidate, X_OK) == 0)
        {
            free(pfx);
            return candidate;
        }
        free(candidate);
        dir = strtok_r(NULL, ":", &saveptr);
    }
    free(pfx);
    return NULL;
}

/* Execute a single command (no pipes) inside child process.
 * Exits the process on error.
 */
static void exec_single(command_t *c)
{
    if (!c || !c->argv || !c->argv[0])
        _exit(127);

    /* handle input redirection */
    if (c->infile)
    {
        int fd = open(c->infile, O_RDONLY);
        if (fd < 0)
        {
            perror("open infile");
            _exit(127);
        }
        if (dup2(fd, STDIN_FILENO) < 0)
        {
            perror("dup2 infile");
            close(fd);
            _exit(127);
        }
        close(fd);
    }

    /* handle output redirection */
    if (c->outfile)
    {
        int flags = O_WRONLY | O_CREAT;
        if (c->append)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;
        int fd = open(c->outfile, flags, 0644);
        if (fd < 0)
        {
            perror("open outfile");
            _exit(127);
        }
        if (dup2(fd, STDOUT_FILENO) < 0)
        {
            perror("dup2 outfile");
            close(fd);
            _exit(127);
        }
        close(fd);
    }

    char *path = which_in_path(c->argv[0]);
    if (!path)
    {
        fprintf(stderr, "osh: command not found: %s\n", c->argv[0]);
        _exit(127);
    }
    execv(path, c->argv);
    perror("execv");
    free(path);
    _exit(127);
}

/* Execute a pipeline (possibly single command). If background == 1, do not wait.
 * rawline is used for job listing.
 */
int execute_pipeline(command_t *cmd, int background, const char *rawline)
{
    if (!cmd)
        return -1;

    /* count commands */
    int n = 0;
    for (command_t *c = cmd; c; c = c->next)
        n++;

    pid_t *pids = calloc(n, sizeof(pid_t));
    if (!pids)
    {
        perror("calloc");
        return -1;
    }

    /* allocate pipes array as pointer-to-int[2] */
    int **pipesfds = NULL;
    if (n > 1)
    {
        pipesfds = calloc(n - 1, sizeof(int *));
        if (!pipesfds)
        {
            free(pids);
            perror("calloc");
            return -1;
        }
        for (int i = 0; i < n - 1; ++i)
        {
            pipesfds[i] = calloc(2, sizeof(int));
            if (!pipesfds[i] || pipe(pipesfds[i]) < 0)
            {
                perror("pipe");
            }
        }
    }

    int idx = 0;
    for (command_t *c = cmd; c; c = c->next, ++idx)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            /* cleanup here if you prefer */
            continue;
        }

        if (pid == 0)
        {
            /* child */
            signal(SIGINT, SIG_DFL);

            /* process group: leader is first child's pid */
            if (idx == 0)
                setpgid(0, 0);
            else
                setpgid(0, pids[0]);

            /* stdin from previous pipe */
            if (idx > 0 && pipesfds)
            {
                dup2(pipesfds[idx - 1][0], STDIN_FILENO);
            }
            /* stdout to next pipe */
            if (c->next && pipesfds)
            {
                dup2(pipesfds[idx][1], STDOUT_FILENO);
            }

            /* close all pipe fds in child */
            if (pipesfds)
            {
                for (int i = 0; i < n - 1; ++i)
                {
                    close(pipesfds[i][0]);
                    close(pipesfds[i][1]);
                }
            }

            exec_single(c);
            _exit(127);
        }
        else
        {
            /* parent */
            if (idx == 0)
                setpgid(pid, pid);
            else
                setpgid(pid, pids[0]);
            pids[idx] = pid;
        }
    }

    /* close pipes in parent */
    if (pipesfds)
    {
        for (int i = 0; i < n - 1; ++i)
        {
            close(pipesfds[i][0]);
            close(pipesfds[i][1]);
            free(pipesfds[i]);
        }
        free(pipesfds);
    }

    pid_t pgid = pids[0];

    if (background)
    {
        add_job(pgid, rawline, JOB_RUNNING);
        printf("[bg] %d\n", pgid);
        free(pids);
        return 0;
    }

    /* put job in foreground */
    tcsetpgrp(STDIN_FILENO, pgid);

    int status = 0;
    for (int i = 0; i < n; ++i)
        waitpid(pids[i], &status, WUNTRACED);

    /* restore shell as foreground */
    tcsetpgrp(STDIN_FILENO, getpgrp());

    if (WIFSTOPPED(status))
        add_job(pgid, rawline, JOB_STOPPED);

    free(pids);
    return 0;
}
