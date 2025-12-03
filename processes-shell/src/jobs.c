#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "jobs.h"

typedef struct job
{
    int jobnum; // 1, 2, 3...
    pid_t pgid; // process group ID
    char *cmdline;
    job_state_t state;
    struct job *next;
} job_t;

static job_t *job_head = NULL;
static int next_jobnum = 1;

void jobs_init(void)
{
    job_head = NULL;
    next_jobnum = 1;
}

void jobs_shutdown(void)
{
    job_t *j = job_head;
    while (j)
    {
        job_t *n = j->next;
        free(j->cmdline);
        free(j);
        j = n;
    }
    job_head = NULL;
}

int add_job(pid_t pgid, const char *cmdline, job_state_t state)
{
    job_t *j = malloc(sizeof(job_t));
    if (!j)
        return -1;
    j->jobnum = next_jobnum++;
    j->pgid = pgid;
    j->cmdline = strdup(cmdline);
    j->state = state;
    j->next = job_head;
    job_head = j;
    return j->jobnum;
}

pid_t get_job_pgid(int jobnum)
{
    for (job_t *j = job_head; j; j = j->next)
        if (j->jobnum == jobnum)
            return j->pgid;
    return -1;
}

void set_job_state(pid_t pgid, job_state_t st)
{
    for (job_t *j = job_head; j; j = j->next)
        if (j->pgid == pgid)
            j->state = st;
}

void remove_job(pid_t pgid)
{
    job_t **pp = &job_head;
    while (*pp)
    {
        if ((*pp)->pgid == pgid)
        {
            job_t *tmp = *pp;
            *pp = tmp->next;
            free(tmp->cmdline);
            free(tmp);
            return;
        }
        pp = &(*pp)->next;
    }
}

void reap_jobs(void)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0)
    {
        for (job_t *j = job_head; j; j = j->next)
        {
            if (j->pgid == pid)
            {
                if (WIFEXITED(status) || WIFSIGNALED(status))
                    j->state = JOB_DONE;
                else if (WIFSTOPPED(status))
                    j->state = JOB_STOPPED;
                else if (WIFCONTINUED(status))
                    j->state = JOB_RUNNING;
                break;
            }
        }
    }
}

void list_jobs(void)
{
    for (job_t *j = job_head; j; j = j->next)
    {
        const char *state =
            (j->state == JOB_RUNNING) ? "Running" : (j->state == JOB_STOPPED) ? "Stopped"
                                                                              : "Done";
        printf("[%d] %d  %s  (%s)\n", j->jobnum, j->pgid, state, j->cmdline);
    }
}
