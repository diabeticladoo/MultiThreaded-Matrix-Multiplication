#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    if(argc!=4){
        exit(-2);
    }
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int c = atoi(argv[3]);
    FILE *f;
    f = fopen("Time_P1.csv", "w");
    fprintf(f, "No. of threads, Time elapsed(in ns)\n");
    fclose(f);
    f = fopen("Time_P2.csv", "w");
    fprintf(f, "No. of threads, Time elapsed(in ns)\n");
    fclose(f);
    for(int i = 1; i <= a*c*10; i++){
        char s[8] = {0};
        sprintf(s,"%d",i);
        if(fork()==0){
            execlp("./P1","./P1",argv[1],argv[2],argv[3],s,(char*)NULL);
        }
        wait(NULL);
        if(fork()==0){
            execlp("./P2","./P2",argv[1],argv[2],argv[3],s,(char*)NULL);
        }
        wait(NULL);
    }
}