#ifndef TP_COMMON_H
#define TP_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>


#define TP_USAGE_STR \
"thread-pool\n" \
"\tex. ./thread-pool <num-workers> <num-items>\n"

#define TP_ERR_L(str) fprintf(stderr, "\e[31m%s\e[0m\n", str);
#define TP_INF_L(str) fprintf(stderr, "\e[32m%s\e[0m\n", str);

#define true (1)
#define false (0)

#define TP_MAX_NSLEEP_RETRY (10)


typedef struct TP_WORKER {
    int id;
    int created;
    pthread_t pid;
} tp_worker_t;


typedef struct TP_JOB {
    int id;
    int done;
} tp_job_t;


typedef struct TP_STATE {
    int worker_c;
    int job_c;
    tp_worker_t* workers;    
    tp_job_t* jobs;
    int c_job_c;
} tp_state_t;


void tp_printjob(tp_job_t* job);
void tp_printworker(tp_worker_t* worker);

/**
initializes a `tp_job_t` struct w/ specified id.
    this function should set the `done` field to `false` or 0
*/
int tp_initjob(tp_job_t* job, int id);

/**
initializes a `tp_worker_t` struct w/ specified id.
    this function should set the `pid` field to the sentinel value -1
*/
int tp_initworker(tp_worker_t* worker, int id);


/**
`calloc` wrapper safely retry when NULL is returned & errno == EINTR.
*/
void* calloc_s(size_t n, size_t size);


#endif

