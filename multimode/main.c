#include "taskset.h"
#include "multimode.h"


int main(int argc, char* argv[])
{
    struct sched_param param;
    pthread_attr_t attr;
    int ret, run;
    int externalInput = 0;
    int taskCount = getTaskCount();
    struct task_struct cTasks[taskCount];  
    struct task_struct cTasks2[taskCount];
    pthread_t thread[taskCount];
    struct thread_info tinfo[taskCount];

    srand (time (NULL));
   // getTasks(oTasks);
    
    generateTasks(cTasks, 0.7);
    copyTaskStruct(cTasks, cTasks2, taskCount);
    struct task_struct* tasks;
    char* fileName= 0;
    for (int i = 0; i < 2; i++){
        if (i == 0){
            tasks = cTasks;
            fileName="fileFPT.txt";
	    rmAssignFPT(tasks);
        }
        else{
            tasks = cTasks2;
            fileName="fileFPM.txt";
	    rmAssignFPM(tasks);
        }
	    setTaskBegin(tasks, taskCount, 2000000000);
	    #ifdef print 
		    printTasks(tasks, taskCount);
	    #endif
	    tasksToFile(tasks, taskCount, fileName);

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
        if (i==0)
            printf("FPT WCRT: %i\n", wcrt);
        else
            printf("FPM WCRT: %i\n", wcrt);
        printf("\n\n");
        wcrt = 0;
    }
    freeTaskStruct(cTasks, taskCount);
    freeTaskStruct(cTasks2, taskCount);
    //for (int i = 0; i < size; i++)
    //    printf("%i", execOrder[i]);
    return ret;
}
