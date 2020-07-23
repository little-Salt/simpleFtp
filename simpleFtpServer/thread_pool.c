#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "thread_pool.h"


typedef struct thread_task
{
	void* (*function) (void*);       
	void*  arg;
    struct thread_task*  next;                       
} thread_task;


typedef struct thread_pool
{
    pthread_mutex_t  lock;
    pthread_cond_t notify;
    pthread_cond_t ildes;

    pthread_t* threads;
    thread_task* first_task;
    thread_task* last_task;
    
    int task_count;
    int size;
    int alives;
    int shutdown;
    int ilde;
} thread_pool;


thread_pool* thread_pool_create(unsigned int pool_size) {
    thread_pool* pool;

    // mallocate mem for pool
    pool = (thread_pool*) malloc(sizeof(struct thread_pool));
    if (pool == NULL) goto err;
    

    pool->task_count = 0;
    pool->size = pool_size;
    pool->ilde = 0;
    pool->alives = 0;
    pool->shutdown = 0;

    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * pool_size);
    pool->first_task = pool->last_task = NULL;

    if (pool->threads == NULL) goto err;

    if ((pthread_mutex_init(&(pool->lock), NULL) != 0) || (pthread_cond_init(&(pool->notify), NULL) != 0) || (pthread_cond_init(&(pool->ildes), NULL) != 0)) goto err;

    for (int i = 0; i < pool_size; ++i) {
        if (pthread_create(&(pool->threads[i]), NULL, thread_pool_thread, (void*)pool) != 0) {
            thread_pool_shutdown(pool);
            return NULL;
        }
        ++(pool->alives);
    }

    return pool;

    err:
        if (pool) {
            thread_pool_free(pool);
        }
        return NULL;
}

static void *thread_pool_thread(void * t_pool) {

    thread_pool *pool = (thread_pool *) t_pool;
    thread_task* task;

    for (;;) {
        pthread_mutex_lock(&(pool->lock));
        ++(pool->ilde);
        if (pool->ilde > pool->task_count) pthread_cond_signal(&(pool->ildes));

        while (pool->task_count == 0 && pool->shutdown != 1) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if (pool->shutdown == 1) break;

        task = pool->first_task;
        --(pool->task_count);
        pool->first_task = task->next;
        if (pool->task_count == 0) pool->last_task = pool->first_task;

        --(pool->ilde);

        pthread_mutex_unlock(&(pool->lock));

        (*(task->function))(task->arg);
        free(task);
    }

    --(pool->alives);

    pthread_mutex_unlock(&(pool->lock));

    pthread_exit(NULL);
}

static int thread_pool_free(thread_pool* pool) {
    if (pool == NULL || pool->alives > 0)
        return -1;
    if (pool->threads != NULL) free(pool->threads);
    thread_task* curr = pool->first_task;
    while(curr != NULL) {
        thread_task* next = curr->next;
        free(curr);
        curr = next;
    }

    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));
    pthread_cond_destroy(&(pool->ildes));

    return 0;
}

int thread_pool_shutdown(thread_pool* pool) {
    if (pool == NULL) return 0;

    if (pthread_mutex_lock(&(pool->lock)) != 0) return -1;

    if (pool->shutdown == 1) return -1;

    pool->shutdown = 1;

    if((pthread_cond_broadcast(&(pool->notify)) != 0) || (pthread_mutex_unlock(&(pool->lock)) != 0)) {
        return -1;
    }

    for(int i = 0; i < pool->size; ++i) {
        if(pthread_join(pool->threads[i], NULL) != 0) {
            return -1;
        }
    }

    return thread_pool_free(pool);;
}


int thread_pool_add_task(thread_pool* pool, void* (*function)(void *), void* arg) {
    if (pool == NULL) return -1;

    if (pool->shutdown == 1) return -1;

    pthread_mutex_lock(&(pool->lock));

    thread_task* new_task = (thread_task*) malloc(sizeof(struct thread_task));
    new_task->function = function;
    new_task->arg = arg;
    new_task->next = NULL;

    if (pool->task_count == 0) {
        pool->first_task = pool->last_task = new_task;
    } else {
        pool->last_task->next = new_task;
    }

    ++(pool->task_count);

    pthread_cond_signal(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}

void thread_pool_wait_idle(thread_pool* pool) {

    pthread_mutex_lock(&(pool->lock));

    while (pool->ilde <= pool->task_count && pool->shutdown != 1) {
        pthread_cond_wait(&(pool->ildes), &(pool->lock));
    }

    pthread_mutex_unlock(&(pool->lock));
}