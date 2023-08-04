#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/shm.h>
#include "helper.h"

int tc = 0;
pthread_mutex_t tc_lock;

typedef struct{
    lli* m1; lli* m2; lli* m3;
    int* chk1; int* chk2;
    int a,b,c,i,j;
}mulParam;
void pack_mulParam(mulParam * mulpar, lli* m1, lli* m2, lli* m3, int* chk1, int* chk2, int a, int b, int c, int i, int j){
    mulpar->m1 = m1;
    mulpar->m2 = m2;
    mulpar->m3 = m3;
    mulpar->chk1 = chk1;
    mulpar->chk2 = chk2;
    mulpar->a = a;
    mulpar->b = b;
    mulpar->c = c;
    mulpar->i = i;
    mulpar->j = j;
}
void unpack_mulParam(mulParam * mulpar, lli** m1, lli** m2, lli** m3, int** chk1, int** chk2, int* a, int* b, int* c, int* i, int* j){
    *m1 = mulpar->m1;
    *m2 = mulpar->m2;
    *m3 = mulpar->m3;
    *chk1 = mulpar->chk1;
    *chk2 = mulpar->chk2;
    *a = mulpar->a;
    *b = mulpar->b;
    *c = mulpar->c;
    *i = mulpar->i;
    *j = mulpar->j;
}

void* mulRoutine(void * par){
    // char debug[1000] = {0};
    mulParam* mulpar = (mulParam*) par;
    // int l = sprintf(debug,"[%d,%d,%d] ",mulpar->b,mulpar->i,mulpar->j);
    lli* m1; lli* m2; lli* m3;
    int* chk1; int* chk2;
    int a,b,c,i,j;
    unpack_mulParam(mulpar,&m1,&m2,&m3,&chk1,&chk2,&a,&b,&c,&i,&j);
    // printf("Running thread for %d,%d [%lld]\n",i,j,m1[0][0]);
    // fflush(stdout);
    lli sum = 0;

    // printf("AAAAAAAAAAAAAA");
    //     printf("%d %d\n", chk1[i], chk2[j]);
    // printf("BBBBBBBBBBBBBBBBB");
    // fflush(stdout);

    while(chk1[i] * chk2[j] == 0){
        // printf("%d %d\n", chk1[i], chk2[j]);
    }
    for(int k = 0; k < b; k++){
        int idxx = i*b+k;
        int idxy = j*b+k;
        lli x = m1[idxx];
        lli y = m2[idxy];
        // l+=sprintf(debug+l,"%lld,%lld ",x,y);
        sum += x * y;
    }
    // printf("%s\t%lld\n",debug,sum);
    // fflush(stdout);
    int idx = i*c+j;
    m3[idx] = sum;

    pthread_mutex_lock(&tc_lock);
    tc--;
    pthread_mutex_unlock(&tc_lock);
    free(mulpar);
    pthread_exit(0);
}

int main(int argc, char *argv[]){
    if(argc<4){
        exit(-2);
    }
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int c = atoi(argv[3]);
    int maxtc = a*c, timed = 0;
    if(argc==5){
        timed = 1;
        if(atoi(argv[4]) <= a*c) maxtc = atoi(argv[4]);
    }else{
        kill(getpid(),SIGSTOP);
    }
    printf("[P2] Max threads allowed = %d\n",maxtc);

    struct timespec tstart;
    struct timespec tend;
    if(timed){
        clock_gettime(CLOCK_MONOTONIC, &tstart);
    }

    // init_rand();
    //random matrix gen for testing 
    // lli* m1 = (lli*) malloc(a*b*sizeof(lli)); 
    // for(int i = 0; i < a; i++) m1[i] = (lli*) malloc(b*sizeof(lli));
    // lli* m2 = (lli*) malloc(b*c*sizeof(lli)); 
    // for(int i = 0; i < c; i++) m2[i] = (lli*) malloc(b*sizeof(lli));
    lli* m3 = (lli*) malloc(a*c*sizeof(lli)); 
    // for(int i = 0; i < a; i++) m3[i] = (lli*) malloc(c*sizeof(lli));
    // mat_randinit(m1,a,b,0,3);
    // mat_randinit(m2,b,c,0,3);
    int key1 = ftok(".", 34);
	int key2 = ftok("..",68); 
    int key3 = ftok(".", 35);
    int key4 = ftok("..",70);
	int shmid1 = shmget(key1,sizeof(lli)*a*(b),0666|IPC_EXCL);
	int shmid2 = shmget(key2,sizeof(lli)*c*(b),0666|IPC_EXCL);
	int shmid3 = shmget(key3,sizeof(int)*a,0666|IPC_CREAT);
	int shmid4 = shmget(key4,sizeof(int)*c,0666|IPC_CREAT);
	lli* m1 = shmat(shmid1, NULL, 0);
	lli* m2 = shmat(shmid2, NULL, 0);
	int* chk1 = shmat(shmid3, NULL, 0);
	int* chk2 = shmat(shmid4, NULL, 0);
    mat_randinit(m3,a,c,0,1);

    // for(int i = 0; i < c; i++){
    //     printf("%d",chk2[i]);
    // }
    // printf("\n");
    // fflush(stdout);

    pthread_t tids[a*c];
    for(int i = 0; i < a; i++){
        for(int j = 0; j < c; j++){
            while(tc>=maxtc); //busy wait for thread count limit
            fflush(stdout);
            pthread_mutex_lock(&tc_lock);
            tc++;
            pthread_mutex_unlock(&tc_lock);
            int tidx = i*c+j;
            mulParam * mulpar = (mulParam*) malloc(sizeof(mulParam));
            pack_mulParam(mulpar,m1,m2,m3,chk1,chk2,a,b,c,i,j);
            // printf("PARAMS: %d,%d,%d\n",mulpar.b,mulpar.i,mulpar.j);
            pthread_create(tids+tidx,NULL,mulRoutine,(void*) mulpar);
        }
    }
    for(int idx = 0; idx < a*c; idx++){
        pthread_join(tids[idx],NULL);
    }
    print_mat(m1,a,b);
    print_mat(m2,c,b);
    print_mat(m3,a,c);

    FILE * f = fopen("out.txt","w");
    write_mat_txt(f,m3,a,c);
    fclose(f);

    // free_mat(m1);
    // free_mat(m2);
    // free_mat(m3);


    shmdt((void *) m1);
	shmdt((void *) m2);
    shmdt((void *) chk1);
    shmdt((void *) chk2);
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);
    shmctl(shmid4, IPC_RMID, NULL);

    if(timed){
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        ulli ttaken = calctime(tstart,tend);
        FILE *f;
        f = fopen("Time_P2.csv", "a");
        // fprintf(f, "No. of threads, Time elapsed(in ns)\n");
        fprintf(f, "%d, %llu\n", maxtc, ttaken);
    }else{
        kill(getppid(),SIGUSR2);
    }

    return 0;
}