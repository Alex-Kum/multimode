#ifndef HELPER_H
#define HELPER_H

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define size 200000
//#define print

typedef void (*funcp)(void* arg);

struct thread_info{
    struct task_struct *tstruct;
    int* input;
    int* running;
    int threadId;
};

struct task_struct{
    int modeCount;
    int* priority;
    struct timespec* period;
    int* execTime;
    struct timespec begin;
    int* limit;
    funcp* function;
};

int wcrt = 0;
int highestPrio = 0;
int lowestPrio = 0;

int g_curUtil = 0;
int g_curTaskCount = 0;
int g_curMode = 0;
int g_curNumber = 0;

char g_curHFPT[18];
char g_curHFPM[18];
char g_curLFPT[18];
char g_curLFPM[18];

void initArray(int* arr, int length){
    for (int i = 0; i < length; i++){
        arr[i] = 0;
    }
} 

double randn(double mu, double sigma){
    double U1, U2, W, mult;
    static double X1, X2;
    static int call = 0;
 
    if (call == 1){
        call = !call;
        return (mu + sigma * (double) X2);
    }
 
    do{
        U1 = -1 + ((double) rand () / RAND_MAX) * 2;
        U2 = -1 + ((double) rand () / RAND_MAX) * 2;
        W = pow (U1, 2) + pow (U2, 2);
    } while (W >= 1 || W == 0);
 
    mult = sqrt ((-2 * log (W)) / W);
    X1 = U1 * mult;
    X2 = U2 * mult;
 
    call = !call;
 
    return (mu + sigma * (double) X1);
}

int refreshFileNames(float util, int taskCount, int number){
    char curHFPT[18] = "rtHFPT";
    char curHFPM[18] = "rtHFPM";
    char curLFPT[18] = "rtLFPT";
    char curLFPM[18] = "rtLFPM";

	char buf[2];
	char buf2[2];
	int u = util*100;
	sprintf(buf, "%i", u);
	sprintf(buf2, "%i", taskCount);
	strcat(curHFPT, buf);
	strcat(curHFPM, buf);
    strcat(curLFPT, buf);
	strcat(curLFPM, buf);
	strcat(curHFPT, buf2);
	strcat(curHFPM, buf2);
    strcat(curLFPT, buf2);
	strcat(curLFPM, buf2);

	char buf3[3];
	sprintf(buf3, "%i", number);
	strcat(curHFPT, "-");
	strcat(curHFPM, "-");
    strcat(curLFPT, "-");
	strcat(curLFPM, "-");
	strcat(curHFPT, buf3);
	strcat(curHFPM, buf3);
    strcat(curLFPT, buf3);
	strcat(curLFPM, buf3);
    printf("%s %s %s\n", buf, buf2, buf3);
    strcat(curHFPT, ".txt\0");
	strcat(curHFPM, ".txt\0");
    strcat(curLFPT, ".txt\0");
	strcat(curLFPM, ".txt\0");

    strncpy(g_curHFPT, curHFPT, 18);
    strncpy(g_curHFPM, curHFPM, 18);
    strncpy(g_curLFPT, curLFPT, 18);
    strncpy(g_curLFPM, curLFPM, 18);

    #ifdef print
        printf("%s, %s, %s, %s\n", g_curHFPT, g_curHFPM, g_curLFPT, g_curLFPM);
	    printf("%s, %s, %s, %s\n", curHFPT, curHFPM, curLFPT, curLFPM);
    #endif
}

/*void wcrtArrToFile(int number, float util, int taskCount, int mode){
    printf("%i\n", mode);
    for (int k = 0; k < 2; k++){
        char* tmp;
        char fptName[20] = "";
        char fpmName[20] = "";
        if (k==0){
            if (g_curMode == 0){
                tmp = "wcrtHFPT";
                sprintf(fptName, "%s", tmp);
            }
            else{
                tmp = "wcrtHFPM";		    
                sprintf(fpmName, "%s", tmp);
            }
        }
        else{
            if (g_curMode == 0){
                tmp = "wcrtLFPT";
                sprintf(fptName, "%s", tmp);
            }
            else{
                tmp = "wcrtLFPM";
		        sprintf(fpmName, "%s", tmp);
            }
            
        }
		char buf[3];
		char buf2[2];
		int u = g_curUtil*10;
		sprintf(buf, "%i", u);
		sprintf(buf2, "%i", g_curTaskCount);
		strcat(fptName, buf);
		strcat(fpmName, buf);
		strcat(fptName, buf2);
		strcat(fpmName, buf2);

		char buf3[3];
		sprintf(buf3, "%i", g_curNumber);
		strcat(fptName, "-");
		strcat(fpmName, "-");
		strcat(fptName, buf3);
		strcat(fpmName, buf3);
		
		strcat(fptName, ".txt\0");
		strcat(fpmName, ".txt\0");
        FILE* f;
        if (g_curMode == 0)
		    f = fopen(fptName, "a");
        else
            f = fopen(fpmName, "a");
		for (int i = 0; i < size; i++){
            if (k==0)
		        fprintf(f, "%i\n", wcrth[i]);
            else
                fprintf(f, "%i\n", wcrtl[i]);
        }
		fclose(f);   
    }
}*/


int compare(const void* a, const void* b){
    return ( *(int*)a - *(int*)b );
}

void printArr(int* arr, int length){
    for (int i = 0; i < length; i++){
    	printf("%i ", arr[i]);
    }
    printf("\n");
}

struct timespec intNsToTime(int time){
    struct timespec ret;
    ret.tv_nsec = time % 1000000000;
    ret.tv_sec = time / 1000000000;

    return ret;
}

int timeToIntNs(struct timespec time){
    int ns = time.tv_sec * 1000000000;
    ns += time.tv_nsec;
    return ns;
}

int sameMode(struct task_struct* tstruct1, int mode1, struct task_struct* tstruct2, int mode2){ //execTime!?
    if(tstruct1->function[mode1] != tstruct2->function[mode2])
    	return 0;

    if(timeToIntNs(tstruct1->period[mode1]) != timeToIntNs(tstruct2->period[mode2]))
    	return 0;

    if (tstruct1->priority[mode1] != tstruct2->priority[mode2])
    	return 0;


    return 1;
}

int removeDuplicates(int* arr, int length){
    int n = length;
    int i = 0;

    while(i < n-1){
        if (arr[i] == arr[i+1]){
            for (int k = i; k < n-1; k++){
            	arr[k] = arr[k+1];
            }
            arr[n-1] = -1;
            n--;
        }
        else{
            i++;
        }
    }
    return n;
}

void removeDuplicatesTask(struct task_struct* tstruct){
	int i = 0;

	while(i < tstruct->modeCount-1){
		if (sameMode(tstruct, i, tstruct, i+1)){
			 for (int k = i; k < tstruct->modeCount-1; k++){
				 tstruct->priority[k] = tstruct->priority[k+1];
				 tstruct->function[k] = tstruct->function[k+1];
				 tstruct->limit[k] = tstruct->limit[k+1];
				 tstruct->execTime[k] = tstruct->execTime[k+1];
				 tstruct->period[k] = tstruct->period[k+1];
				 tstruct->priority[k] = tstruct->priority[k+1];
			 }
			 tstruct->modeCount--;
		}
		else{
			i++;
		}

	}
}

void printTimespec(const char* str, struct timespec t){
    printf("%s", str);
    printf("%li : %li\n", t.tv_sec, t.tv_nsec);
}

void initTaskStruct(struct task_struct* tstruct, int taskCount, int* modes){
    for(int i = 0; i < taskCount; i++){
        tstruct[i].modeCount = modes[i];
        tstruct[i].priority = (int*) malloc(modes[i]*sizeof(int));
        tstruct[i].period = (struct timespec*) malloc(modes[i]*sizeof(struct timespec));
        tstruct[i].limit = (int*) malloc(modes[i]*sizeof(int));
        tstruct[i].function = malloc(modes[i]*sizeof(funcp));
        tstruct[i].execTime = (int*) malloc(modes[i]*sizeof(int));
    }

}

void freeTaskStruct(struct task_struct* tstruct, int taskCount){
    for (int i = 0; i < taskCount; i++){
        free(tstruct[i].priority);
        free(tstruct[i].period);
        free(tstruct[i].limit);
        free(tstruct[i].function);
        free(tstruct[i].execTime);
    }
}

void printTaskStruct(struct task_struct* m){
    printf("ModeCount: %i\n", m->modeCount);

    for (int i = 0; i < m->modeCount; i++){
        printf("Mode: %i \n" , i);
        printf("Priority: %i \n", m->priority[i]);
        printf("Limit: %i \n", m->limit[i]);
        printTimespec("Period: ", m->period[i]);
        printf("ExecTime: %i \n", m->execTime[i]);
        printTimespec("Begin: ", m->begin);
        printf("\n");
    }
}

void printTasks(struct task_struct* t, int taskCount){
    for (int i = 0; i < taskCount; i++){
        printTaskStruct(&t[i]);
    }
}

void tasksToFile(struct task_struct* t, int taskCount, char* fname){
    FILE *f = fopen(fname, "w");

    for (int i = 0; i < taskCount; i++){
        fprintf(f, "ModeCount: %i\n", t[i].modeCount);

        for (int k = 0; k < t[i].modeCount; k++){
	    fprintf(f, "Mode: %i \n" , k);
	    fprintf(f, "Priority: %i \n", t[i].priority[k]);
	    fprintf(f, "Limit: %i \n", t[i].limit[k]);
	    fprintf(f, "Period: %i\n", timeToIntNs(t[i].period[k]));
	    fprintf(f, "ExecTime: %i \n", t[i].execTime[k]);
	    fprintf(f, "Begin: %i\n", timeToIntNs(t[i].begin));
	    fprintf(f, "\n\n");
        }
    }
    fclose(f);
}

void wcrtToFile(int wcrt, char* fname){
    FILE *f = fopen(fname, "a");
    fprintf(f, "%i\n", wcrt);
    fclose(f);
}

void copyTaskStruct(struct task_struct* tstruct, struct task_struct* newStruct, int taskCount){
    int modes[taskCount];
    for(int i = 0; i < taskCount; i++)
        modes[i] = tstruct[i].modeCount;

    initTaskStruct(newStruct, taskCount, modes);
    for (int k = 0; k < taskCount; k++){
        newStruct[k].modeCount = tstruct[k].modeCount;
        newStruct[k].begin = tstruct[k].begin;
        for (int i = 0; i < newStruct[k].modeCount; i++){
            newStruct[k].priority[i] = tstruct[k].priority[i];
            newStruct[k].period[i] = tstruct[k].period[i];
            newStruct[k].execTime[i] = tstruct[k].execTime[i];
            newStruct[k].limit[i] = tstruct[k].limit[i];
            newStruct[k].function[i] = tstruct[k].function[i];
        }
    }
}

int getRand(int min, int max){
    return (rand () % ((max + 1) - min)) + min;
}

struct timespec diff(struct timespec start, struct timespec end){
    struct timespec tmp;
    tmp.tv_sec = end.tv_sec-start.tv_sec;
    tmp.tv_nsec = end.tv_nsec-start.tv_nsec;

    if (tmp.tv_nsec < 0){
        tmp.tv_nsec += 1000000000;
        tmp.tv_sec -= 1;
    }
    return tmp;
}

struct timespec add(struct timespec t1, struct timespec t2){
    struct timespec tmp;
    tmp.tv_nsec = t1.tv_nsec + t2.tv_nsec;
    tmp.tv_sec = t1.tv_sec + t2.tv_sec;

    if (tmp.tv_nsec > 1000000000){
        tmp.tv_nsec -= 1000000000;
        tmp.tv_sec += 1;
    }
    return tmp;
}

struct timespec mySleep(int nanosec){
    struct timespec t = intNsToTime(nanosec);
    nanosleep(&t, &t);
}

void fillThreadInfos(struct thread_info* tinfo, struct task_struct* tstruct, int taskCount, int* run, int* externalInput){   
    for (int i = 0; i < taskCount; i++){
        tinfo[i].input = externalInput;
        tinfo[i].running = run;
        tinfo[i].tstruct = &tstruct[i]; 
        tinfo[i].threadId = i;
    }
}

int getMode(struct task_struct* tstruct, int externalI){
    for (int i = 0; i < tstruct->modeCount; i++){
        if (externalI <= tstruct->limit[i]){
            return i;
        }
    }
    return -1;
}
#endif
