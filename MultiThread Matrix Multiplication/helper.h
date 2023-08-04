#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <sys/resource.h>

#define lli long long int
#define ulli unsigned lli
#define MAXSZ 300

void init_rand(){
    srand((unsigned)time(NULL));
}

int randint(int l, int r){
    return rand()%(r-l)-l;
}

void mat_randinit(lli* m, int a, int b, int l, int r){
    // m = (lli*) malloc(a*sizeof(lli*));
    for(int i = 0; i < a; i++){
        // m[i] = (lli*) malloc(b*sizeof(lli));
        for(int j = 0; j < b; j++){
            int idx = i*b+j;
            m[idx] = randint(l,r);
            // printf("%lld",m[i][j]);
        }
    }
}

void print_mat(lli* m, int a, int b){
    for(int i = 0; i < a; i++){
        for(int j = 0; j < b; j++){
            int idx = i*b+j;
            lli e = m[idx];
            printf("%lld ",e);
        }
        printf("\n");
        fflush(stdout);
    }
    printf("[%d rows, %d columns]\n",a,b);
    fflush(stdout);
}

void read_mat_txt(FILE * f, lli m[][MAXSZ], int a, int b, int transpose){
    for(int i = 0; i < a; i++){
        for(int j = 0; j < b; j++){
            if(transpose)
                fscanf(f, "%lld ", &m[j][i]);
            else
                fscanf(f, "%lld ", &m[i][j]);
        }
    }
}

void write_mat_txt(FILE * f, lli* m, int a, int b){
    for(int i = 0; i < a; i++){
        for(int j = 0; j < b; j++){
            int idx = i*b+j;
            lli e = m[idx];
            fprintf(f,"%lld ",e);
        }
        fprintf(f,"\n");
    }
}

void free_mat(lli* m){
    free(m);
}

// void schedInfo(){
//     int policy = sched_getscheduler(getpid());
//     struct rlimit rl;
//     getrlimit(RLIMIT_RTPRIO,&rl);
//     printf("Policy: %d\nLims: %ld %ld\n",policy,rl.rlim_cur,rl.rlim_max);
// }

void sleepns(lli ns){
    struct timespec t;
    t.tv_nsec = ns;
    t.tv_sec = ns/1e9;
    nanosleep(&t,NULL);
}

ulli calctime(struct timespec tstart, struct timespec tend){
    ulli start,end,ttaken;
    start = (ulli)(tstart.tv_sec) + (ulli)(tstart.tv_nsec);
    end = (ulli)(tend.tv_sec) + (ulli)(tend.tv_nsec);
    if((tstart.tv_nsec) > (tend.tv_nsec))
        ttaken = ((ulli)(1e9) - start) + (end);
    else
        ttaken = end - start;
    return ttaken;
}

#endif