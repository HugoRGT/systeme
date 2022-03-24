#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char *arg[],char **envp){
    size_t i = 0;
    while(envp[i] != NULL){
        printf("%s\n",envp[i]);
        i++;
    }
    return 0;
}