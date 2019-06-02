#include "taskset.h"
#include "multimode.h"


int main(int argc, char* argv[])
{
    struct sched_param param;
    pthread_attr_t attr;
    int ret, run;
    int externalInput = 0;
    int taskCount = getTaskCount();
    struct task_struct tasks[taskCount];  
    struct task_struct oTasks[taskCount];
    pthread_t thread[taskCount];
    struct thread_info tinfo[taskCount];

    srand (time (NULL));

    //initTaskStruct(tasks, taskCount, modeCount);
    getTasks(oTasks);

    rmAssign(oTasks, tasks, taskCount);
    setTaskBegin(tasks, taskCount, 2000000000);
    printTasks(tasks, taskCount);

    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        printf("mlockall failed: %m\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

    ret = pthread_attr_init(&attr);
    if (ret) {
        printf("init pthread attributes failed\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
        printf("pthread setstacksize failed\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

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

    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        printf("pthread setinheritsched failed\n");
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

    fillThreadInfos(tinfo, tasks, taskCount, &run, &externalInput);
    run=1;

    ret = createThreads(tinfo, thread, taskCount, &attr);
    if (ret){
        freeTaskStruct(tasks, taskCount);
        exit(-2);
    }

    for (int i = 0; i < 500000000; i++){
        externalInput = getRand(0,9000);
    }
    run = 0;

    joinThreads(thread, taskCount);

    freeTaskStruct(tasks, taskCount);
    return ret;
}
