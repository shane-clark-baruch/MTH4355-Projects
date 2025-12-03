// include/shell.h
#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>

/* A single command in a pipeline */
typedef struct command
{
    char **argv;          /* NULL-terminated argv list */
    char *infile;         /* input redirection file or NULL */
    char *outfile;        /* output redirection file or NULL */
    int append;           /* 1 if >> was used */
    struct command *next; /* next command in pipeline (NULL if last) */
} command_t;

/* Parser: parse a line into a linked list of command_t (pipeline).
 * Caller must free the returned command chain via free_command_chain()
 * Returns NULL on parse error or empty line.
 */
command_t *parse_line(const char *line);

/* Free parser result */
void free_command_chain(command_t *cmd);

/* Executor: execute a pipeline (cmd points to head). If background==1,
 * do not wait for pipeline to finish and register job.
 * Returns 0 on success, -1 on error.
 */
int execute_pipeline(command_t *cmd, int background, const char *rawline);

#endif /* SHELL_H */
