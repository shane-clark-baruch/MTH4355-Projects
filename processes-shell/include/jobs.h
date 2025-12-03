// include/jobs.h
#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

void jobs_init(void);
void jobs_shutdown(void);
void add_job(pid_t pgid, const char *cmdline);
void reap_jobs(void);
void list_jobs(void);

#endif /* JOBS_H */
