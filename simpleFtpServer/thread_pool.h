#ifndef _THREAD_POOL_H_

#define _THREAD_POOL_H_

typedef struct  thread_pool thread_pool;

typedef struct thread_task thread_task;

/*
 * Function: thread_pool_create 
 * --------------------
 * create a thread pool with given size
 *
 *  pool_size: unnegetive size of thread pool
 *
 *  returns: new thread_pool pointer
 */
thread_pool* thread_pool_create(unsigned int pool_size);

/*
 * Function: thread_pool_shutdown 
 * --------------------
 * try to shutdown the thread pool, would wait all worker finish current job
 *
 *  pool: pointer to the thread_pool
 *
 *  returns: 0 means success shutdown, elseise -1
 */
int thread_pool_shutdown(thread_pool* pool);

/*
 * Function: thread_pool_wait_idle 
 * --------------------
 * block caller thread, wait until at least one available worker
 *
 *  pool: pointer to the thread_pool
 *
 *  returns: no return value
 */
void thread_pool_wait_idle(thread_pool* pool);

/*
 * Function: thread_pool_add_task 
 * --------------------
 * add task to the queue and notify current idle worker
 *
 *  pool: pointer to the thread_pool
 *  function: the function pointer wait for excute
 *  arg: arguments pointer pass to function
 *
 *  returns: 0 means success shutdown, elseise -1
 */
int thread_pool_add_task(thread_pool* pool, void* (*function)(void *), void* arg);

/*
 * Function: thread_pool_thread 
 * --------------------
 * the worker job whith the thread pool, each worker would get job from task queue,
 * if no task in the queue, would wait until notify
 *
 *  pool: pointer to the thread_pool
 *
 *  returns: no return value
 */
static void *thread_pool_thread(void * t_pool);

/*
 * Function: thread_pool_free 
 * --------------------
 * free resources used by thread pool
 *
 *  pool: pointer to the thread_pool
 *
 *  returns: 0 means success, elsewise -1
 */
static int thread_pool_free(thread_pool* pool);

#endif