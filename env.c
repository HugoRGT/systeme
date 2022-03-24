#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern char **environ;
int main(int argc,char *arg[]){
    size_t i = 0;
    while(environ[i] != NULL){
        printf("%s\n",environ[i]);
        i++;
    }
    return 0;
}