#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "helper.h"

int r1 = 1, r2 = 1;

void handle_sigusr1(int sig){
    r1 = 0;
    return;
}
void handle_sigusr2(int sig){
    r2 = 0;
    return;
}

int main(int argc, char *argv[]){
    signal(SIGUSR1,handle_sigusr1);
    signal(SIGUSR2,handle_sigusr2);

    if(argc!=4){
        exit(-2);
    }

    pid_t pid1, pid2;
    lli quantum = 2*1000000;

    if((pid1=fork())==0){
        execlp("./P1","./P1",argv[1],argv[2],argv[3],(char*)NULL);
    }else if((pid2=fork())==0){
        execlp("./P2","./P2",argv[1],argv[2],argv[3],(char*)NULL);
    }else{
        //P1 and P2 immediately pause on exec (SIGSTOP selves)
        sleepns(quantum);
        while(r1+r2>0){
            if(r1){
                // printf("P1\n");
                kill(pid1,SIGCONT); //unpause P1
                sleepns(quantum); //after quantum
                kill(pid1,SIGSTOP); //pause P1
            }
            if(r2){
                // printf("P2\n");
                kill(pid2,SIGCONT); //unpause P2
                sleepns(quantum); //after quantum
                kill(pid2,SIGSTOP); //pause P2
            }
        } //P1 ends and signals SIGUSR1 to S, P2 signals SIGUSR2 -> loop breaks
        kill(pid1,SIGCONT); kill(pid2,SIGCONT);
        wait(NULL);
        wait(NULL);
    }
    return 0;
}