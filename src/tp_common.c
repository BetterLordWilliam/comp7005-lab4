#include "tp_common.h"


void tp_printjob(tp_job_t* job)
{
    fprintf(stderr, "Job: { id: %d, done: %d }\n",
        job->id, job->done);
}
    
void tp_printworker(tp_worker_t* worker)
{
    fprintf(stderr, "Worker: { id: %d }\n",
        worker->id);
}


int tp_initjob(tp_job_t* job, int id)
{
    if (id < 0) {
        return -1;
    }
    job->id = id;
    job->done = false;
    return 0;
}

int tp_initworker(tp_worker_t* worker, int id)
{
    if (id < 0) {
        return -1;
    }
    worker->id = id;
    worker->pid = -1;
    return 0;
}


void* calloc_s(size_t n, size_t size)
{
    void* mptr;
    do {
        mptr = calloc(n, size);
        if (mptr == NULL && errno == EINTR)
            continue; // [WO] retry the allocation on NULL w/ EINTR
        else if (mptr == NULL)
            return NULL;
        else
            return mptr;
    } while (true);
}


