// src/shell.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include "shell.h"
#include "jobs.h"

/* SIGCHLD handler: delegate to jobs subsystem to reap and update states. */
static void sigchld_handler(int sig)
{
    (void)sig;
    reap_jobs();
}

/* We want the shell to ignore Ctrl-C and Ctrl-Z itself; children will restore defaults.
 * You can optionally implement a no-op handler for SIGTSTP, but ignoring is fine.
 */
static void sigtstp_ignore(int sig)
{
    (void)sig;
    /* intentionally empty: shell itself ignores Ctrl-Z */
}

/* Helper: trim trailing whitespace and detect/strip a trailing '&' for background */
static int strip_trailing_ampersand(char *line)
{
    if (!line)
        return 0;
    int L = (int)strlen(line);
    /* trim trailing whitespace */
    while (L > 0 && (line[L - 1] == ' ' || line[L - 1] == '\t'))
    {
        line[--L] = '\0';
    }
    if (L > 0 && line[L - 1] == '&')
    {
        line[L - 1] = '\0';
        /* trim whitespace again in case there was space before & */
        while (L > 0 && (line[L - 1] == ' ' || line[L - 1] == '\t'))
        {
            line[--L] = '\0';
        }
        return 1;
    }
    return 0;
}

int main(void)
{
    char *line = NULL;
    size_t cap = 0;
    ssize_t len;

    /* Initialize jobs subsystem and install SIGCHLD handler */
    jobs_init();
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    /* Shell ignores Ctrl-C and Ctrl-Z; children will reset to defaults. */
    signal(SIGINT, SIG_IGN);         // shell ignores Ctrl-C
    signal(SIGTSTP, sigtstp_ignore); // shell ignores Ctrl-Z
    signal(SIGTTOU, SIG_IGN);

    while (1)
    {
        printf("osh> ");
        fflush(stdout);

        len = getline(&line, &cap, stdin);
        if (len == -1)
        {
            if (feof(stdin))
            {
                /* Ctrl-D / EOF */
                printf("\n");
                break;
            }
            perror("getline");
            continue;
        }

        /* remove trailing newline */
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        /* detect background operator (&) at end and strip it */
        int background = strip_trailing_ampersand(line);

        /* parse the line into a pipeline of command_t */
        command_t *cmd = parse_line(line);
        if (!cmd)
            continue; /* empty line or parse error */

        /* quick access to first command's argv for builtins */
        char **argv = cmd->argv;

        if (argv && argv[0])
        {
            /* built-in: exit */
            if (strcmp(argv[0], "exit") == 0)
            {
                free_command_chain(cmd);
                break;
            }

            /* built-in: cd */
            if (strcmp(argv[0], "cd") == 0)
            {
                if (argv[1])
                {
                    if (chdir(argv[1]) != 0)
                        perror("cd");
                }
                else
                {
                    char *home = getenv("HOME");
                    if (home)
                        chdir(home);
                }
                free_command_chain(cmd);
                continue;
            }

            /* built-in: jobs */
            if (strcmp(argv[0], "jobs") == 0)
            {
                list_jobs();
                free_command_chain(cmd);
                continue;
            }

            /* built-in: fg %N */
            if (strcmp(argv[0], "fg") == 0)
            {
                if (!argv[1] || argv[1][0] != '%')
                {
                    printf("Usage: fg %%jobnum\n");
                }
                else
                {
                    int jobnum = atoi(argv[1] + 1);
                    pid_t pgid = get_job_pgid(jobnum);
                    if (pgid < 0)
                    {
                        printf("No such job\n");
                    }
                    else
                    {
                        /* give terminal to job and continue it */
                        tcsetpgrp(STDIN_FILENO, pgid);
                        kill(-pgid, SIGCONT);
                        /* wait for any process in the group */
                        int status;
                        waitpid(-pgid, &status, WUNTRACED);
                        /* restore terminal to shell */
                        tcsetpgrp(STDIN_FILENO, getpgrp());
                        /* if stopped, job handler (reap_jobs) or code below will add it */
                    }
                }
                free_command_chain(cmd);
                continue;
            }

            /* built-in: bg %N */
            if (strcmp(argv[0], "bg") == 0)
            {
                if (!argv[1] || argv[1][0] != '%')
                {
                    printf("Usage: bg %%jobnum\n");
                }
                else
                {
                    int jobnum = atoi(argv[1] + 1);
                    pid_t pgid = get_job_pgid(jobnum);
                    if (pgid < 0)
                    {
                        printf("No such job\n");
                    }
                    else
                    {
                        kill(-pgid, SIGCONT);
                        set_job_state(pgid, JOB_RUNNING);
                    }
                }
                free_command_chain(cmd);
                continue;
            }
        }

        /* otherwise execute the pipeline (execute_pipeline handles background & job registration) */
        int rc = execute_pipeline(cmd, background, line);
        if (rc < 0)
        {
            fprintf(stderr, "osh: failed to execute command\n");
        }

        free_command_chain(cmd);
    } /* end while */

    free(line);
    jobs_shutdown();
    return 0;
}
