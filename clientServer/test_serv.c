#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include "message.h"

// standard input (file descriptor 0) and standard output (file descriptor 1) were redirected to fifos before loading this program

int main(int argc, char *argv[]){
  for(int i = 0; i < argc; ++i){
    fprintf(stderr, "argv[%d]  = %s\n", i, argv[i]);
  }

  // send "server to client" to the client
  int r1 = send_string(1, "server to client");
  if (r1 < 0){
    fprintf(stderr, "Error send_string \"server to client\"\n");
    exit(1);
  }

  // receive n from the client
  int n;
  ssize_t r2 = read(0, &n, sizeof(int));
  if (r2 == -1){
    perror("read n");
    exit(1);
  }
  if (r2 == 0){
    fprintf(stderr, "End of file detected while reading n\n");
    exit(1);
  }
  fprintf(stderr, "n = %d\n", n);

  // receive a string from the client
  char *res = recv_string(0);
  if (!res){
    fprintf(stderr, "Error recv_string\n");
    exit(1);
  }
  fprintf(stderr, "res = %s\n", res);
  free(res);
  res = NULL;

  return 0;
}
