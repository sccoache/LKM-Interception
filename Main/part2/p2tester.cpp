#include <sys/syscall.h>
#include <iostream>
#include <iomanip>
//#include "processinfo_usr.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include "ancestrystruct.h"
#include <unistd.h>

using namespace std;

#define __NR_cs3013_syscall2 378

long testCall2(void);

/**
 * Print a stat returned from the syscall.
 **/
void printStat(char const *stat, long long val)
{
    cout << left << setw(20) << stat << right << setw(20) << val << endl;
}

int main(){
    int pid1, pid2;
    //printf("\tcs3103_syscall2: %ld\n", testCall2());

    if ((pid1 = fork()) < 0){
        printf("Fork Failed\n");
    }
    else if (pid1 == 0){ //is child
        sleep(5);
        exit(0);
    }
    else //is parent
    {
        if ((pid2 = fork()) < 0){
            printf("Fork Failed\n");
        }
        else if (pid2 == 0){ //is child
            sleep(5);
            exit(0);
        } 
        else{
            printf("I am a parent process. Running __NR_cs3013_syscall2\n\n");
            
            struct ancestry* ancstruct; // = (struct ancestry*)malloc(sizeof(struct ancestry));
            //struct ancestry* ancstruct = (struct ancestry*)malloc(sizeof(struct ancestry));

            long ret = (long) syscall(__NR_cs3013_syscall2, &pid2, ancstruct);
    
            if(ret == -1){
                printf("Error Occured\n");
                return ret;
            }else{
                printf("-----Printing process genealogy-----\n");
                printf("-----Target-----\n");
                printf("[1] Target ID: %i\n", (int)ancstruct.target[0]);
                
                printf("-----Children-----\n");
                for(int q = 0; q < 99; q++){
                    if(ancstruct.children[q] < 1){
                        break;
                    }
                    printf("[%i]Child ID: %i\n", q+1, ancstruct.children[q]);
                }
                printf("-----Ancestors-----\n");
                for(int w = 0; w < 99; w++){
                    if(ancstruct.ancestors[w] < 1){
                        break;
                    }
                    else{
                        printf("[%i]Ancestor ID: %i\n", w+1, (int)ancstruct.ancestors[w]);
                    }
                }
                printf("-----Siblings-----\n");
                for(int e = 0; e < 9; e++){
                    if(ancstruct.siblings[e] < 1){
                        break;
                    }
                    else{
                        printf("[%i]Sibling ID: %i\n", e+1, (int)ancstruct.siblings[e]);
                    }
                }
                
            }
            exit(0);
        }
    }
    return 0;
}

long testCall2(void){
    return (long) syscall(__NR_cs3013_syscall2);
}
