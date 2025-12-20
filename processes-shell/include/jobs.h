// include/jobs.h
#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

typedef enum
{
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE
} job_state_t;

void jobs_init(void);
void jobs_shutdown(void);

int add_job(pid_t pgid, const char *cmdline, job_state_t state);
void reap_jobs(void);

void list_jobs(void);
pid_t get_job_pgid(int jobnum);
void set_job_state(pid_t pgid, job_state_t st);
void remove_job(pid_t pgid);

#endif /* JOBS_H */
