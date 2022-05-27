#define _D_DEFAULT_SOURCE
#include "message.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>



int send_string(int fd, char *str){
  ssize_t len = strlen(str)+1;
  write(fd,&len,sizeof(ssize_t));
  write(fd,str,len);
  return 0;
}


//allocates a char * and returns the received string
char *recv_string(int fd){
  ssize_t len;
  ssize_t ret = read(fd,&len,sizeof(ssize_t));
  if (ret < 0){exit(1);}
  char *buf = calloc(len+1,sizeof(char));
  read(fd,buf,len*sizeof(char));
  buf[len]='\0';
  return buf;
}




int send_argv(int fd, char *argv[]){
  ssize_t len = 0;
  while(argv[len]!=NULL){
    len++;
  }

  write(fd,&len,sizeof(ssize_t));
  for (size_t i = 0; i<len;i++){
    send_string(fd,argv[i]);
  }
  return 0;
}


char **recv_argv(int fd){
  size_t len;
  read(fd,&len,sizeof(ssize_t));
  //printf("received argv len : %zu\n",len);

  char **tab=calloc(len+1,sizeof(char*));//allocate +1 for NULL

  for(size_t i = 0; i<len;i++){
    tab[i]=recv_string(fd);
    //printf("'%s'\n",tab[i]);
  }

  tab[len]=NULL;//Add NULL at the end
  return tab;

}

void destroy_argv(char **argv,int len){
  for (int i = 0; i<len;i++){
    if(argv[i]!=NULL){
      free(argv[i]);
    }
  }
  free(argv);
}
