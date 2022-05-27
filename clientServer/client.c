#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "message.h"

#define BUFSIZE 1024


volatile int usr1_receive = 0;
volatile int usr2_receive = 0;
volatile int sigint_receive =0;


void handler(int sig){
  if (sig == SIGUSR1){
    usr1_receive=1;
  }
  if (sig == SIGUSR2){
    usr2_receive = 1;
  }
  return;
}

void print_usage(){
  fprintf(stderr,"Usage : ./client <game> [args]...\n");
}

int file_exists(char *path,int flag){
  if( access( path, flag ) != -1){
    //printf("%s is found\n",path);
    return 1;
  }
  printf("%s is not found\n",path);

  return 0;
}


int main(int argc, char **argv){
  //set up signal signal_handler
  char cwd[BUFSIZE];
  getcwd(cwd,sizeof(cwd));
  char environ_path[BUFSIZE+8];
  sprintf(environ_path,"%s:%s",getenv("PATH"),cwd);
  setenv("PATH",environ_path,1);

  if (argc <2){print_usage();return 1;}
  char game_path[BUFSIZE+8];
  sprintf(game_path,"%s_cli",argv[1]);


  if(!file_exists(game_path,X_OK)){
    fprintf(stderr, "The game you requested doesn't exist\n");
    return 1;
  }


  struct sigaction action;
  sigemptyset(&(action.sa_mask));
  action.sa_handler = &handler;
  action.sa_flags = 0;

  if (sigaction(SIGUSR2,&action , NULL) == -1){
    perror("sigaction");
    exit(1);
  }
  if (sigaction(SIGUSR1,&action , NULL) == -1){
    perror("sigaction");
    exit(1);
  }


  pid_t server_pid;
  if (!file_exists("/tmp/game_server.pid",O_RDONLY)){
    fprintf(stderr,"Communication with the server couldn't be established\n");
    return 1;
  }
  FILE *fp = fopen("/tmp/game_server.pid","r");
  if (fp==NULL){
    perror("fopen");
  }
  fread(&server_pid,sizeof(pid_t),1,fp);
  fclose(fp);

  kill(server_pid,SIGUSR1);

  int pipe_fd = open("/tmp/game_server.fifo",O_WRONLY);
  if (pipe_fd<0){
    perror("open");
  }

  char mypid[7];
  sprintf(mypid,"%d",getpid());
  char cli_fifo0[BUFSIZE]; //cli_0.fifo path
  char cli_fifo1[BUFSIZE]; //cli_01fifo path
  sprintf(cli_fifo0,"/tmp/game_server/cli%s_0.fifo",mypid);
  sprintf(cli_fifo1,"/tmp/game_server/cli%s_1.fifo",mypid);


  send_string(pipe_fd,mypid);
  argv++;
  send_argv(pipe_fd,argv);
  close(pipe_fd);
  pause();
  if(usr1_receive){
    usr1_receive = 0;
    fprintf(stderr,"RECEIVED SIGNAL SIGUSR1\n");
    int fd_0 = open(cli_fifo0,O_WRONLY);
    int fd_1 = open(cli_fifo1,O_RDONLY);
    if (fd_1 <0 || fd_0 <0){fprintf(stderr, "An error occured\n");return 1;}
    argv[0]=game_path;
    // int ret = dup2(fd_0,3);
    // if (ret == -1){
    //   perror("dup2");
    //   return 1;
    // }
    // close(fd_0);
    // ret = dup2(fd_1,4);
    // if (ret == -1){
    //   perror("dup2");
    //   return 1;
    // }
    // close(fd_1);
    dup2(fd_0,3);
    dup2(fd_1,4);
    //printf("Execve argv0 : %s argv1 : %s\n",argv[0],argv[1]);
    if (!execvp(argv[0],&argv[0])){
      perror("execvp");
    }
    return 1;

  }
  if(usr2_receive){
    usr2_receive = 0;
    printf("RECEIVED SIGNAL SIGUSR2\n");
    fprintf(stderr,"An error occured, terminating now\n");
    return 1;

  }






  return 0;
}
