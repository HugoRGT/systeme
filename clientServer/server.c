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
volatile int sigint_receive = 0;
volatile int sigchld_receive = 0;


struct child_node{
  pid_t pid;
  pid_t client_pid;
  struct child_node *next;
};



int file_exists(char *path,int flag){
  if( access( path, flag ) != -1){
    //printf("%s is found\n",path);
    return 1;
  }
  printf("%s is not found\n",path);

  return 0;
}

int dir_exists(char *path){
  DIR *dir = opendir(path);
  if(dir){
    closedir(dir);
    return 1;
  }else if(errno == ENOENT){
    return 0;
  }
  return 1;
}




void handler(int sig){
  if (sig == SIGUSR1){
    usr1_receive = 1;
  }
  if (sig == SIGUSR2){
    usr2_receive = 1;
  }
  if (sig == SIGINT){
    sigint_receive = 1;
  }
  if (sig == SIGCHLD){
    sigchld_receive++;
  }
  return;
}


void delete_tmp_files(struct child_node *rem){
  char path[BUFSIZE];
  sprintf(path,"/tmp/game_server/cli%d_0.fifo",rem->client_pid);
  if (remove(path)<0){perror("remove");}
  sprintf(path,"/tmp/game_server/cli%d_1.fifo",rem->client_pid);
  if (remove(path)<0){perror("remove");}
  return;

}

void kill_childs(struct child_node *first){
  int status;
  if (first->pid == 0){
    free(first);
    return;
  }
  struct child_node *curr = first;
  struct child_node *tmp = curr;
  while (curr != NULL){
    tmp = curr->next;
    printf("Kill child pid : %d\n",curr->pid);
    kill(curr->pid,SIGTERM);
    waitpid(curr->pid,&status,0);
    delete_tmp_files(curr);
    free(curr);
    curr = tmp;
  }
}

void pid_list_print(struct child_node *first){
  printf("PRINTING LIST\n");
  if (first->pid == 0){
    return;
  }
  struct child_node *curr = first;
  while(curr != NULL){
    printf("pid %d\n",curr->pid);
    curr=curr->next;
  }
}

void free_argv(char **argv){
  if (argv == NULL){return;}
  int i = 0;
  char *curr = argv[i];
  //printf("freeing argv\n");
  while(curr!=NULL){
    //printf("freeing %s\n",curr);
    free(curr);
    i++;
    curr = argv[i];
  }
}
int pid_list_remove(struct child_node *first, pid_t rem){
  if (first->pid == rem){
    delete_tmp_files(first);
    first->pid = 0;
    return 1;
  }
  pid_list_print(first);
  printf("attempt to remove pid %d\n",rem);
  struct child_node *curr = first;
  struct child_node *anchor = curr;
  while(curr != NULL){
    curr=curr->next;
    if (curr != NULL && curr->pid == rem){
      anchor ->next = curr->next;
      delete_tmp_files(curr);
      free(curr);
      return 1;
    }
    anchor = curr;
  }
  return 0;
}

void pid_list_append(struct child_node *first,pid_t pid,pid_t client){
  //printf("Adding %d to list\n",pid);

  if (first->pid ==0){
    first->pid = pid;
    first->next = NULL;
    first->client_pid = client;
    return;
  }
  struct child_node *new = calloc(1,sizeof(struct child_node));
  new->pid = pid;
  new->next = NULL;
  new->client_pid = client;
  struct child_node *curr = first;
  while(curr->next!=NULL){
    curr = curr->next;
  }
  curr->next = new;
  //printf("Done\n");

}


int main(){
  char cwd[BUFSIZE];
  getcwd(cwd,sizeof(cwd));




  FILE *fp;
  if(file_exists("/tmp/game_server.pid",F_OK)){
    fprintf(stderr, "The server is already online\n");
    return 1;
  }
  fp = fopen("/tmp/game_server.pid","w");
  pid_t pid = getpid();
  printf("pid = %d\n",pid);
  fwrite(&pid,1,sizeof(pid_t),fp);
  fclose(fp);

  //Pipe
  if(! file_exists("/tmp/game_server.fifo",F_OK)){
    mkfifo("/tmp/game_server.fifo",0666);
  }
  if (! dir_exists("/tmp/game_server")){
    if (mkdir("/tmp/game_server",S_IRWXU)==-1){
      perror("mkdir");
      exit(1);
    }
    printf("Created dir \n");
  }

  //sigset_t set;
  //sigemptyset(&set);
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
  if (sigaction(SIGINT,&action , NULL) == -1){
    perror("sigaction");
    exit(1);
  }
  if (sigaction(SIGCHLD,&action , NULL) == -1){
    perror("sigaction");
    exit(1);
  }

  char **cli_argv = NULL;
  char *cli_pid_str = NULL;
  int pipe_fd;
  int status;
  struct child_node *pid_list = calloc(1,sizeof(struct child_node));


  while (1){
    if(usr1_receive){ //NEW GAME START
      usr1_receive = 0;
      printf("\nRECEIVED SIGNAL SIGUSR1\n");
      pipe_fd = open("/tmp/game_server.fifo", O_RDONLY);
      if (pipe_fd < 0){
        perror("open");
      }
      cli_pid_str = recv_string(pipe_fd);
      cli_argv = recv_argv(pipe_fd);
      close(pipe_fd);
      pid_t cli_pid = (pid_t) atoi(cli_pid_str);
      char cli_fifo0[BUFSIZE]; //cli_0.fifo path
      char cli_fifo1[BUFSIZE]; //cli_01fifo path
      char game_str[BUFSIZE+8];//game_file_path (+8 for the '_serv part' )
      sprintf(cli_fifo0,"/tmp/game_server/cli%d_0.fifo",cli_pid);
      sprintf(cli_fifo1,"/tmp/game_server/cli%d_1.fifo",cli_pid);
      //printf("client pid : %d, path : %s\n",cli_pid,cli_fifo0);

      sprintf(game_str,"%s/%s_serv",cwd,cli_argv[0]);
      free(cli_argv[0]);
      cli_argv[0]=game_str;

      mkfifo(cli_fifo0,0666);
      mkfifo(cli_fifo1,0666); //create client pipes
      if (!file_exists(game_str,X_OK)){
        kill(cli_pid,SIGUSR2);
        printf("Inalid game name '%s'\n",game_str);
      }else{
        //printf("Game is ok\n");
        kill(cli_pid,SIGUSR1); //tell client the pipes are ready

        pid_t child = fork(); //fork child process

        if (child == -1) {
          fprintf(stderr,"Error\n");
          perror("fork");
          exit(1);
        }


        if (child == 0){
          //fprintf(stderr,"Opening fifo pipes..\n");
          int fd_0 = open(cli_fifo0,O_RDONLY);
          int fd_1 = open(cli_fifo1,O_WRONLY);
          if (fd_1 <0 || fd_0 <0){fprintf(stderr, "An error occured\n");continue;}
          int ret = dup2(fd_0,0);
          if (ret == -1){
            perror("dup2");
            continue;
          }
          close(fd_0);

          ret = dup2(fd_1,1);
          if (ret == -1){
            perror("dup2");
            continue;
          }
          close(fd_1);

          //printf("exec : %s\n",cli_argv[0]);
          execvp(game_str,cli_argv);
          perror("execvp");
          exit(1);
        }else{
          pid_list_append(pid_list,child,cli_pid);
          free_argv(&cli_argv[1]);
          free(cli_argv);
          free(cli_pid_str);
        }

      }



    }
    if(usr2_receive){
      usr2_receive = 0;
      printf("\nRECEIVED SIGNAL SIGUSR2\n");

    }
    if (sigint_receive){
      sigint_receive = 0;
      printf("\nRECEIVED SIGINT\n");
      remove("/tmp/game_server.pid");
      kill_childs(pid_list);
      return 1;
    }
    if(sigchld_receive){
      printf("\nRECEIVED SIGCHLD\n");
      sigchld_receive--;

      pid_t term_child = waitpid(-1,&status,0);
      printf("Result of remove : %d\n",pid_list_remove(pid_list,term_child));

    }

  }






  return 0;
}
