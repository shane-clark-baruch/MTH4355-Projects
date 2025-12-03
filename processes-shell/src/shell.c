#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAXTOK 256

/* Reap children to avoid zombies */
static void sigchld_handler(int sig) {
    (void)sig;
    int saved = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
    errno = saved;
}

/* Check if last token is & (background) */
int is_background(char **argv, int argc) {
    if (argc > 0 && argv[argc-1] && strcmp(argv[argc-1], "&") == 0) return 1;
    return 0;
}

int main(void) {
    char *line = NULL;
    size_t cap = 0;
    ssize_t len;

    /* install SIGCHLD handler to reap background children */
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    /* ignore SIGINT in the shell; children inherit default */
    signal(SIGINT, SIG_IGN);

    while (1) {
        printf("osh> ");
        fflush(stdout);

        len = getline(&line, &cap, stdin);
        if (len == -1) {
            /* EOF (Ctrl-D) or error */
            if (feof(stdin)) {
                printf("\n");
                break;
            } else {
                perror("getline");
                continue;
            }
        }

        if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';

        /* simple whitespace tokenization (no quoting support yet) */
        char *saveptr;
        char *tok = strtok_r(line, " \t", &saveptr);
        char *argv[MAXTOK];
        int argc = 0;
        while (tok && argc < MAXTOK-1) {
            argv[argc++] = tok;
            tok = strtok_r(NULL, " \t", &saveptr);
        }
        argv[argc] = NULL;

        if (argc == 0) continue; /* empty line */

        /* builtins */
        if (strcmp(argv[0], "exit") == 0) {
            break;
        }
        if (strcmp(argv[0], "cd") == 0) {
            if (argc > 1) {
                if (chdir(argv[1]) != 0) perror("cd");
            } else {
                char *home = getenv("HOME");
                if (home) chdir(home);
            }
            continue;
        }

        /* background? */
        int background = is_background(argv, argc);
        if (background) {
            argv[argc-1] = NULL;
            argc--;
        }

        /* handle simple redirection tokens: >, >>, <  (single redirect only) */
        int redirect_out = 0, redirect_append = 0, redirect_in = 0;
        char *out_file = NULL, *in_file = NULL;
        for (int i = 0; i < argc; ++i) {
            if (strcmp(argv[i], ">") == 0 && i+1 < argc) {
                redirect_out = 1;
                out_file = argv[i+1];
                argv[i] = NULL;
                argc = i;
                break;
            } else if (strcmp(argv[i], ">>") == 0 && i+1 < argc) {
                redirect_append = 1;
                out_file = argv[i+1];
                argv[i] = NULL;
                argc = i;
                break;
            } else if (strcmp(argv[i], "<") == 0 && i+1 < argc) {
                redirect_in = 1;
                in_file = argv[i+1];
                argv[i] = NULL;
                argc = i;
                break;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            
            signal(SIGINT, SIG_DFL);

            /* setup I/O redirection if requested */
            if (redirect_in && in_file) {
                int fd = open(in_file, O_RDONLY);
                if (fd < 0) { perror("open input"); exit(1); }
                if (dup2(fd, STDIN_FILENO) < 0) { perror("dup2"); exit(1); }
                close(fd);
            }
            if ((redirect_out || redirect_append) && out_file) {
                int flags = O_WRONLY | O_CREAT;
                if (redirect_append) flags |= O_APPEND;
                else flags |= O_TRUNC;
                int fd = open(out_file, flags, 0644);
                if (fd < 0) { perror("open output"); exit(1); }
                if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2"); exit(1); }
                close(fd);
            }

            /* exec the program */
            execvp(argv[0], argv);
            /* if exec fails: */
            perror("execvp");
            exit(127);
        } else {
            /* parent */
            if (!background) {
                int status;
                waitpid(pid, &status, 0);
            } else {
                printf("[bg] pid %d\n", pid);
               
            }
        }
    } /* end loop */

    free(line);
    return 0;
}
