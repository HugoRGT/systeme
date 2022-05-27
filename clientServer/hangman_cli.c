#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "message.h"

// file descriptors associated to the fifos
// these values may be different in your program
#define SERV_IN_FILENO 4
#define SERV_OUT_FILENO 3

// the standard file descriptors (0, 1 et 2) are associated to the launch terminal

int main(int argc, char *argv[]){
  for(int i = 0; i < argc; ++i){
    fprintf(stderr, "argv[%d]  = %s\n", i, argv[i]);
  }
  // receive a string from the server
  fprintf(stderr,"Start\n");
  char *res = recv_string(SERV_IN_FILENO);
  if (!res){
    fprintf(stderr, "Error recv_string\n");
    exit(1);
  }
  fprintf(stderr, "res = %s\n", res);
  free(res);
  res = NULL;

  return 0;
}
