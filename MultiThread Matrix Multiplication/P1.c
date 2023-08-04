#include <stdio.h>
#include <stdlib.h>
// #include <time.h>
// #include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/shm.h>
// #include <sys/ipc.h> 
// #include <sys/msg.h> 
#include "helper.h"
 
 
lli *p1;
lli *p2;
int *chk1;
int *chk2;
 
lli a1[MAXSZ][MAXSZ];
lli a2[MAXSZ][MAXSZ];
 
typedef struct{
    int skip;
    int read;
    int col;
    lli *p;
    int fnum;
}Param;
 
 
void *reading(void* param){
 
 
    Param* par=(Param*) param;
    int b=par->fnum;
 
    // printf("Thread enter %d\n",b);
 
    FILE * f1;
    lli *p;
    lli *a;
    int *chk;
    if(par->fnum==1)
    {
        //f1 = fopen ("input1.txt", "r");
        p=p1;
        a = &a1[0][0];
        chk = chk1;
    }
    else
    {
        //f1 = fopen ("input2.txt", "r");
        p=p2;
        a = &a2[0][0];
        chk = chk2;
    }
    int n=par->skip,m=n;
    /* char line[MAXSZ];
 
    while(m && fgets(line, MAXSZ, f1))
    {
        m--;
    } */
 
    n=par->read;
    int lines=0;
    int col=par->col;
    int j=(par->skip)*(col);
 
 
 
    while(lines<n)
    {
        for(int i=0;i<col;i++)
        {
            //fscanf(f1, "%d ",(p+(j++)));
            *(p+(j++)) = *(a + MAXSZ*(m+lines) + i);
        }
        // j+=col;
        chk[m+lines] = 1;
        lines++;
        // *(p+j++) = 1;
        // j++;
        
    }
 
    //fclose(f1);
    // printf("thread done %d\n",b);
 
    pthread_exit(0);
 
}
 
 
 
 
 
int main(int argc, char *argv[]){
    // sleep(5);
    if(argc<4){
        exit(-2);
    }
    int sz1 = atoi(argv[1]);
    int sz2 = atoi(argv[2]);
    int sz3 = atoi(argv[3]);
    // printf("SIZES: %d %d %d",sz1,sz2,sz3);
    int maxtc1 = sz1, maxtc2 = sz3, timed = 0;
    if(argc==5){
        timed = 1;
        if(atoi(argv[4]) <= sz1) maxtc1 = atoi(argv[4]);
        if(atoi(argv[4]) <= sz3) maxtc2 = atoi(argv[4]);
    }else{
        kill(getpid(),SIGSTOP);
    }
    printf("[P1] Max threads allowed for file 1: %d\n",maxtc1);
    printf("[P1] Max threads allowed for file 2: %d\n",maxtc2);
    
    //PREPROCESSING
    FILE *f;
    f = fopen ("matrix1.txt", "r");
    read_mat_txt(f,a1,sz1,sz2,0);
    fclose(f);
    f = fopen("matrix2.txt", "r");
    read_mat_txt(f,a2,sz2,sz3,1);
    fclose(f);

    struct timespec tstart;
    struct timespec tend;
    if(timed){
        clock_gettime(CLOCK_MONOTONIC, &tstart);
    }

    int key1 = ftok(".", 34);
    int key2 = ftok("..",68);
    int key3 = ftok(".", 35);
    int key4 = ftok("..",70);
 
    lli *arr1;
    lli *arr2;
    // int *arr2;
 
	int shmid1 = shmget(key1,sizeof(lli)*(sz1*(sz2)),0666|IPC_CREAT);
	int shmid2 = shmget(key2,sizeof(lli)*(sz3*(sz2)),0666|IPC_CREAT);
	int shmid3 = shmget(key3,sizeof(int)*sz1,0666|IPC_CREAT);
	int shmid4 = shmget(key4,sizeof(int)*sz3,0666|IPC_CREAT);
 
	arr1 = (lli *)shmat(shmid1, NULL, 0);
    p1=arr1;
 
    arr2 = (lli *)shmat(shmid2, NULL, 0);
    p2=arr2;

    chk1 = (int*)shmat(shmid3, NULL, 0);
    chk2 = (int*)shmat(shmid4, NULL, 0);
 
    pthread_t tids1[maxtc1];
    pthread_t tids2[maxtc2];
 
 
    int x1=(int)(sz1/maxtc1);
    int y1=(int)(sz1%maxtc1);
 
    int x2=(int)(sz3/maxtc2);
    int y2=(int)(sz3%maxtc2);
 
    int b1=0,b2=0;
 
    Param par1[maxtc1];
    Param par2[maxtc2];
 
    int matcmm;
    // max(maxtc1,maxtc2);
    if(maxtc1>maxtc2)
    {
       matcmm=maxtc1; 
    }
    else
    {
        matcmm=maxtc2;
    }
 
    for(int a=0;a<matcmm;a++)
    {
        /* FILE *f1;
        f1 = fopen ("input1.txt", "r");
        FILE *f2;
        f2 = fopen ("input2.txt", "r"); */
 
        if(a<maxtc1)
        {
            par1[a].skip=b1;
            par1[a].col=sz2;
            par1[a].p=p1;
            par1[a].fnum=1;
            if(y1>0)
            {
                par1[a].read=x1+1;
                y1--;
                b1+=x1+1;
            }
            else
            {
                par1[a].read=x1;
                b1+=x1;
            }
            // printf("%d %d %d \n",a,par1[a].skip,par1[a].read);
            {
                pthread_create(tids1+a,NULL,reading,par1+a);
            }
        }
 
        if(a<maxtc2)
        {
            par2[a].skip=b2;
            par2[a].col=sz2;
            par2[a].p=p2;
            par2[a].fnum=2;
            if(y2>0)
            {
                par2[a].read=x2+1;
                y2--;
                b2+=x2+1;
            }
            else
            {
                par2[a].read=x2;
                b2+=x2;
            }
            // printf("%d %d %d \n",a,par2[a].skip,par2[a].read);
            {
                pthread_create(tids2+a,NULL,reading,par2+a);
            }
        }
 
    }
 
 
    for(int i = 0; i < maxtc1; i++){
        pthread_join(tids1[i],NULL);
    }
 
    for(int i = 0; i < maxtc2; i++){
        pthread_join(tids2[i],NULL);
    }
    shmdt((void *) arr1);
    shmdt((void *) arr2);
    shmdt((void *) chk1);
    shmdt((void *) chk2);

    if(timed){
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        ulli ttaken = calctime(tstart,tend);
        FILE *f;
        f = fopen("Time_P1.csv", "a");
        // fprintf(f, "No. of threads, Time elapsed(in ns)\n");
        fprintf(f, "%d, %llu\n", maxtc1+maxtc2, ttaken);
    }else{
        kill(getppid(),SIGUSR1);
    }
 
    return 0;
}