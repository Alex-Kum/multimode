#include "taskset.h"
#include "multimode.h"


int main(int argc, char* argv[])
{
    const int taskCount = 2;
    const int modeCount = 3;

    struct sched_param param;
    pthread_attr_t attr;
    int ret, run;
    int externalInput = 0;

    pthread_t thread[taskCount];
    struct task_struct tasks[taskCount];
    struct thread_info tinfo[taskCount];
    
    srand (time (NULL));

    initTaskStruct(tasks, taskCount, modeCount);
    getTasks(tasks);
    rmAssign(tasks, taskCount, modeCount);
    printTasks(tasks, taskCount, modeCount);

    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        printf("mlockall failed: %m\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
        printf("init pthread attributes failed\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
        printf("pthread setstacksize failed\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    if (ret) {
        printf("pthread setschedpolicy failed\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }    
     
    param.sched_priority = 98;

    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
        printf("pthread setschedparam failed\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        printf("pthread setinheritsched failed\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

    fillThreadInfos(tinfo, tasks, taskCount, &run, &externalInput);
    run=1;

    /* Create a pthread with specified attributes */
    ret = createThreads(tinfo, thread, taskCount, &attr);
    if (ret){
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

    for (int i = 0; i < 1000000000; i++){
        externalInput = getRand(0,9000);
    }
    run = 0;

    /* Join the thread and wait until it is done */
    joinThreads(thread, taskCount);
    
    freeTaskStruct(tasks, taskCount);
    return ret;
}