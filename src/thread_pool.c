#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <poll.h>

#include "tp_common.h"


pthread_mutex_t tp_jobqueue_a = PTHREAD_MUTEX_INITIALIZER;


int tp_dowork()
{
    return 0;
}


int main(int argc, char** argv)
{
    if (argc != 3) {
        TP_ERR_L("unexpected number of arguments.");
        fprintf(stdout, TP_USAGE_STR);
        goto error;
    }

    // parse the arguments
    tp_state_t app = { 0 };
    app.worker_c = atoi(argv[1]);
    if (!(app.worker_c > 0)) {
        goto error;
    }
    app.job_c = atoi(argv[2]);
    if (!(app.job_c > 0)) {
        goto error;
    }
    fprintf(stdout, "starting tp w/ %d workers & %d items\n",
        app.job_c, app.worker_c);


    // initialize job structs
    app.jobs = NULL;
    app.jobs = (tp_job_t*)calloc(app.job_c, sizeof(tp_job_t));
    if (app.jobs == NULL) {
        goto error;
    } // [WO] retry on `errno`
    for (int i = 0; i < app.job_c; i++) {
        tp_initjob((app.jobs + i), i);
        tp_printjob((app.jobs + i));
    }

    // initialzie worker structs
    app.workers = NULL;
    app.workers = (tp_worker_t*)calloc(app.worker_c, sizeof(tp_worker_t));
    if (app.workers == NULL) {
        goto error;
    } // [WO] retry on `errno` EINTR
    for (int i = 0; i < app.worker_c; i++) {
        tp_initworker((app.workers + i), i);
        tp_printworker((app.workers + i));
    }


    // kick-off worker threads


    // join worker threads


    free(app.jobs);
    free(app.workers);

    return 0;

error:
    if (app.jobs != NULL)
        free(app.jobs);
    if (app.workers != NULL)
        free(app.workers);
    
    return 1;
}

