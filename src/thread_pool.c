#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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
    int strack = 0;
    int strack_c = 0;

    struct timespec tp_worker_rem = { 0 };

    tp_worker_t* worker = (tp_worker_t*) arg;

    while (app.c_job_c < app.job_c) {
        pthread_mutex_lock(&tp_jobqueue_a); // so this blocks until the mutex is lockable
        // save the job address
        tp_job_t* j = ( app.jobs + app.c_job_c );
        // advance a processed counter, which is like advancing through
        // a queue
        // threads use this to get the currently unprocessed thing
        app.c_job_c += 1;
        pthread_mutex_unlock(&tp_jobqueue_a);   // release so other threads can do stuff
       
        // the simulated work
        // continues invoking until nansleep returns 0
        // other nanosleep returns write to `tp_worker_rem` 
        strack = nanosleep(&tp_worker_delay, &tp_worker_rem);
        do {
            if (strack_c == TP_MAX_NSLEEP_RETRY) break; // just in case
            strack = nanosleep(&tp_worker_rem, &tp_worker_rem); // keep sleeping until the entire thing is done
            if (strack != 0 && errno != EINTR)
                break; // this is a legitimate error case w/ `nanosleep`
            strack_c++;
        } while (strack != 0);
    
        // after work is done log that job is processed
        j->done = true;
        fprintf(stdout, "worker=%d JOB_DONE job=%d\n",
            worker->id, j->id);

        // tp_printjob(j);
    }

    pthread_exit(NULL);
    return NULL;
}


int main(int argc, char** argv)
{
    int pcR, pjR;
    pcR = pjR = 0;

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
    app.jobs = (tp_job_t*)calloc_s(app.job_c, sizeof(tp_job_t));
    if (app.jobs == NULL) {
        goto error;
    }
    for (int i = 0; i < app.job_c; i++) {
        tp_initjob((app.jobs + i), i);
    }

    // initialzie worker structs
    app.workers = NULL;
    app.workers = (tp_worker_t*)calloc_s(app.worker_c, sizeof(tp_worker_t));
    if (app.workers == NULL) {
        goto error;
    }
    for (int i = 0; i < app.worker_c; i++) {
        tp_initworker((app.workers + i), i);
        pcR = pthread_create(&(app.workers + i)->pid, NULL, tp_dowork,
            (void*) (app.workers + i));
        if (pcR != 0) {
            fprintf(stderr, "failed to created thread for worker w/ id %d\n", i);
            continue;
        }
        (app.workers + i)->created = true;
    }

    // join worker threads
    for (int i = 0; i < app.worker_c; i++) {
        if ((app.workers + i)->created != true)
            continue;
        pjR = pthread_join((app.workers + i)->pid, NULL);
        if (pjR != 0) {
            fprintf(stderr, "failed to join thread for worker w/ id %d\n", i);
        }
    }

    // check that all jobs are done
    fprintf(stderr, "\nin main thread before exiting check that all jobs are done:\n");
    for (int i = 0; i < app.job_c; i++) {
        tp_printjob((app.jobs + i));
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

