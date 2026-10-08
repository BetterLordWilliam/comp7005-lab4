#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// #include <unistd.h>
#include <time.h>

#include "tp_common.h"

struct timespec tp_worker_delay = {
    .tv_sec     = 2,
    .tv_nsec    = 0
};

tp_state_t app = { 0 };
pthread_mutex_t tp_jobqueue_a = PTHREAD_MUTEX_INITIALIZER;


void* tp_dowork(void* arg)
{
    tp_worker_t* worker = (tp_worker_t*) arg;

    // so I need to acquire the lock
    // if I acquire the lock, advance the pointer (so that the boys
    // can also get work done, release the lock cause I don't need it
    // anymore this job is mine) -- I think...
    // fprintf(stdout, "I am: %d\n", worker->id);
    // fprintf(stdout, "I am unsafely reading job 0 state: %d\n",
    //    (app.jobs + 0)->done);

    while (app.c_job_c < app.job_c) { // oo busy wait. but it should be fine
        pthread_mutex_lock(&tp_jobqueue_a); // so this blocks until the mutex is locked
        // save the job address
        tp_job_t* j = ( app.jobs + app.c_job_c );
        // ~~advance the pointer~~ no that messes w/ free later, advance a processed counter
        // threads use this to get the currently unprocessed thing
        app.c_job_c += 1;
        pthread_mutex_unlock(&tp_jobqueue_a);   // release so other threads can do stuff
        
        fprintf(stdout, "I am %d, I have got job %d\n",
            worker->id, j->id);
        
        // simulate some work being done w/ that stuff
        // why is this not defined?
        // sleep(2);
        nanosleep(&tp_worker_delay, NULL);
    }

    pthread_exit(NULL);
    return NULL;
}


int main(int argc, char** argv)
{
    if (argc != 3) {
        TP_ERR_L("unexpected number of arguments.");
        fprintf(stdout, TP_USAGE_STR);
        goto error;
    }

    // parse the arguments
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
        // tp_printjob((app.jobs + i));
    }

    // initialzie worker structs
    app.workers = NULL;
    app.workers = (tp_worker_t*)calloc(app.worker_c, sizeof(tp_worker_t));
    if (app.workers == NULL) {
        goto error;
    } // [WO] retry on `errno` EINTR
    for (int i = 0; i < app.worker_c; i++) {
        tp_initworker((app.workers + i), i);
        // tp_printworker((app.workers + i));

        pthread_create(&(app.workers + i)->pid, NULL, tp_dowork,
            (void*) (app.workers + i));
    }

    // join worker threads
    for (int i = 0; i < app.worker_c; i++) {
        pthread_join((app.workers + i)->pid, NULL);
    }


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

