#ifndef TP_COMMON_H
#define TP_COMMON_H

#include <stdio.h>
#include <pthread.h>


#define TP_USAGE_STR \
"thread-pool\n" \
"\tex. ./thread-pool <num-workers> <num-items>\n"

#define TP_ERR_L(str) fprintf(stderr, "\e[31m%s\e[0m\n", str);
#define TP_INF_L(str) fprintf(stderr, "\e[32m%s\e[0m\n", str);

#define true (1)
#define false (0)


typedef struct TP_WORKER {
    int id;
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


// void tp_init_worker(tp_job_t* tp_worker_t);

// void tp_init_job(tp_job_t* tp_job);

// int tp_dowork();

void tp_printjob(tp_job_t* job);
void tp_printworker(tp_worker_t* worker);

int tp_initjob(tp_job_t* job, int id);
int tp_initworker(tp_worker_t* worker, int id);


#endif

