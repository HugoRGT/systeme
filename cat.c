#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv){
    if(argc <= 1){
        exit(EXIT_FAILURE);
    }
    size_t i = 1;

    while(i < argc){
        FILE* file = fopen(argv[i],"r");
        if(file == NULL){
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        
    }
}