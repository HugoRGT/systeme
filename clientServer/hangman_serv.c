#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "message.h"
#include <time.h>
#define WORDSIZEMAX 27

// standard input (file descriptor 0) and standard output (file descriptor 1) were redirected to fifos before loading this program



int getRandom(int nbMax){
  int num = (rand()%(nbMax-1+1))+1;
  return num;
}



int getNbLigne(FILE *f){
  int count = 0;
  char buf[WORDSIZEMAX];
  while(fgets(buf,sizeof(buf),f) != NULL){
    ++count;
  }
  return count;
}



char* getWordRandom(int line){
  FILE *f = fopen("dictionnaire.txt","r");
  int count = 1;
  char *buf = malloc(sizeof(WORDSIZEMAX));
  fprintf(stderr,"%d\n",line);
  while(fgets(buf,sizeof(buf),f) != NULL && count < line){
    ++count;
  }
  fprintf(stderr,"%s\n",fgets(buf,sizeof(buf),f));
  fclose(f);
  return buf;
}


int main(int argc, char *argv[]){
  for(int i = 0; i < argc; ++i){
    fprintf(stderr, "argv[%d]  = %s\n", i, argv[i]);
  }
  if(argc > 3 || argc == 2){
    int r1 = send_string(1, "Error : Wrong number of arguments\nUsage : ./client hangman [-n N]");
    if (r1 < 0){
      fprintf(stderr, "Error send_string \"Error : Wrong number of arguments\nUsage : ./client hangman [-n N]\"\n");
      exit(1);
    }
  }
  if(strcmp(argv[1],"-n") != 0){
    int r1 = send_string(1, "Error : Option is not supported\nUsage : ./client hangman [-n N]");
    if (r1 < 0){
      fprintf(stderr, "Error send_string \"Error : Wrong number of arguments\nUsage : ./client hangman [-n N]\"\n");
      exit(1);
    }
  }



  FILE *dico = fopen("dictionnaire.txt","r");
  if(dico == NULL){
    perror("Unable to open file");
    exit(1);
  }

  srand(time(NULL));

  int nbLigne = getNbLigne(dico);//NB LIGNE DANS FICHIER
  int line = getRandom(nbLigne);//LIGNE CHOISI AU HASARD

  char *selected = malloc(sizeof(WORDSIZEMAX));
  selected = getWordRandom(line);
  fprintf(stderr,"%s\n",selected);
  free(selected);


  fclose(dico);
  /*char selected[WORDSIZEMAX];
  ssize_t len = sizeof(char);
  char *line = malloc(len);*/
  

  return 0;
}
