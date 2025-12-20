// src/parser.c
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "shell.h"

/* Allocate safe duplicate */
static char *strdup_safe(const char *s)
{
    if (!s)
        return NULL;
    char *r = strdup(s);
    if (!r)
        perror("strdup");
    return r;
}

/* === TOKENIZATION WITH QUOTES & ESCAPES ================================== */

typedef struct
{
    char **items;
    int count;
    int cap;
} token_list_t;

static void tokens_init(token_list_t *t)
{
    t->items = NULL;
    t->count = 0;
    t->cap = 0;
}

static void tokens_push(token_list_t *t, char *tok)
{
    if (t->count + 1 >= t->cap)
    {
        t->cap = t->cap ? t->cap * 2 : 8;
        t->items = realloc(t->items, sizeof(char *) * t->cap);
    }
    t->items[t->count++] = tok;
}

static void tokens_free(token_list_t *t)
{
    for (int i = 0; i < t->count; i++)
        free(t->items[i]);
    free(t->items);
}

/* Tokenizer that handles quotes and escapes */
static int tokenize(const char *line, token_list_t *out)
{
    tokens_init(out);
    const char *p = line;

    while (*p)
    {
        while (isspace((unsigned char)*p))
            p++;

        if (!*p)
            break;

        /* quoted string */
        if (*p == '"')
        {
            p++;
            char buf[4096];
            int bi = 0;

            while (*p && *p != '"')
            {
                if (*p == '\\' && p[1] == '"')
                {
                    buf[bi++] = '"';
                    p += 2;
                }
                else
                {
                    buf[bi++] = *p++;
                }
            }

            if (*p != '"')
            {
                fprintf(stderr, "osh: unmatched double quote\n");
                tokens_free(out);
                return -1;
            }
            p++; /* skip closing quote */
            buf[bi] = '\0';
            tokens_push(out, strdup_safe(buf));
        }

        /* single quote */
        else if (*p == '\'')
        {
            p++;
            char buf[4096];
            int bi = 0;
            while (*p && *p != '\'')
            {
                buf[bi++] = *p++;
            }
            if (*p != '\'')
            {
                fprintf(stderr, "osh: unmatched single quote\n");
                tokens_free(out);
                return -1;
            }
            p++; /* skip closing quote */
            buf[bi] = '\0';
            tokens_push(out, strdup_safe(buf));
        }

        /* special tokens */
        else if (*p == '|' || *p == '<' || *p == '>')
        {
            if (*p == '>' && p[1] == '>')
            {
                tokens_push(out, strdup_safe(">>"));
                p += 2;
            }
            else
            {
                char tmp[2] = {*p, '\0'};
                tokens_push(out, strdup_safe(tmp));
                p++;
            }
        }

        /* normal word */
        else
        {
            char buf[4096];
            int bi = 0;
            while (*p &&
                   !isspace((unsigned char)*p) &&
                   *p != '|' && *p != '<' && *p != '>' &&
                   *p != '\'' && *p != '"')
            {
                buf[bi++] = *p++;
            }
            buf[bi] = '\0';
            tokens_push(out, strdup_safe(buf));
        }
    }
    return 0;
}

/* === PARSE TOKENS INTO COMMAND STRUCTURES ================================ */

command_t *parse_line(const char *line)
{
    if (!line)
        return NULL;

    token_list_t toks;
    if (tokenize(line, &toks) < 0)
        return NULL;

    if (toks.count == 0)
    {
        tokens_free(&toks);
        return NULL;
    }

    command_t *head = NULL;
    command_t *cur = NULL;

    int i = 0;
    while (i < toks.count)
    {
        /* start a new command if needed */
        if (!cur)
        {
            cur = calloc(1, sizeof(command_t));
            if (!cur)
            {
                perror("calloc");
                tokens_free(&toks);
                return NULL;
            }
            cur->argv = NULL;
            cur->next = NULL;

            if (!head)
                head = cur;
            else
            {
                command_t *tmp = head;
                while (tmp->next)
                    tmp = tmp->next;
                tmp->next = cur;
            }
        }

        char *tok = toks.items[i];

        /* PIPE */
        if (strcmp(tok, "|") == 0)
        {
            if (!cur->argv)
            {
                fprintf(stderr, "osh: syntax error near unexpected token '|'\n");
                tokens_free(&toks);
                return NULL;
            }
            cur = NULL; /* start new command next loop */
            i++;
            continue;
        }

        /* REDIRECTION */
        if (strcmp(tok, "<") == 0)
        {
            if (i + 1 >= toks.count)
            {
                fprintf(stderr, "osh: missing filename after '<'\n");
                tokens_free(&toks);
                return NULL;
            }
            cur->infile = strdup_safe(toks.items[i + 1]);
            i += 2;
            continue;
        }

        if (strcmp(tok, ">") == 0 || strcmp(tok, ">>") == 0)
        {
            int append = (tok[1] == '>');
            if (i + 1 >= toks.count)
            {
                fprintf(stderr, "osh: missing filename after '%s'\n", tok);
                tokens_free(&toks);
                return NULL;
            }
            cur->outfile = strdup_safe(toks.items[i + 1]);
            cur->append = append;
            i += 2;
            continue;
        }

        /* Reg. argument: append into argv */
        int argc = 0;
        if (cur->argv)
            while (cur->argv[argc])
                argc++;

        char **newargv = calloc(argc + 2, sizeof(char *));
        if (cur->argv)
        {
            for (int j = 0; j < argc; j++)
                newargv[j] = cur->argv[j];
            free(cur->argv);
        }
        newargv[argc] = strdup_safe(tok);
        newargv[argc + 1] = NULL;
        cur->argv = newargv;

        i++;
    }

    tokens_free(&toks);

    return head;
}

/* Free linked command pipeline */
void free_command_chain(command_t *cmd)
{
    while (cmd)
    {
        command_t *next = cmd->next;
        if (cmd->argv)
        {
            for (int i = 0; cmd->argv[i]; i++)
                free(cmd->argv[i]);
            free(cmd->argv);
        }
        if (cmd->infile)
            free(cmd->infile);
        if (cmd->outfile)
            free(cmd->outfile);
        free(cmd);
        cmd = next;
    }
}
