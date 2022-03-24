#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


void printenv(const char *name){
    char *c = getenv(name);
    if(c == NULL){
        printf("Error. Variable doesn't exists\n");
        return;
    }
    printf("%s\n",c);    
    
}

int main(int argc,char *arg[]){
    printenv("HOME");
    printenv("LANG");
    printenv("PATH");
    printenv("PWD");
    printenv("SHELL");
    printenv("USER");
    return 0;
}