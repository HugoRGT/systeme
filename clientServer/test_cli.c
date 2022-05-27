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

  // send b to the server
  int b = 42;
  sleep(atoi(argv[1]));
  //fprintf(stderr,"Sending n to server\n");
  ssize_t r1 = write(SERV_OUT_FILENO, &b, sizeof(int));
  if (r1 == -1){
    perror("write b");
    exit(1);
  }

  // send "server to client" to the server
  int r2 = send_string(SERV_OUT_FILENO, "client to server");
  if (r2 == -1){
    fprintf(stderr, "Error send_string \"client to server\"\n");
    exit(1);
  }

  return 0;
}
