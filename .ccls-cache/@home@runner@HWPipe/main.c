#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_1 "/tmp/fifo1to2"
#define FIFO_2 "/tmp/fifo2to1"

#define MAX_RBUF 80

int FIFO1to2, FIFO2to1;

static void sig_end(){
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  int pid, nbytes;
  char rbuf[MAX_RBUF] = "";

  if(argc < 2)
  {
    fprintf(stderr,"Your input is invalid\n");
    fprintf(stderr," Please input 1 or 2\n");
    fprintf(stderr,"--------------------------------\n");
    exit(EXIT_FAILURE);
  }
  if((strcmp(argv[1],"1")!=0 && strcmp(argv[1],"2")!=0)|| argc >2)
  {
    fprintf(stderr,"Your input is invalid\n");
    fprintf(stderr," Please input 1 or 2\n");
    fprintf(stderr,"--------------------------------\n");
    exit(EXIT_FAILURE);
  }

  if(access(FIFO_1,F_OK) == -1){
    FIFO1to2 = mkfifo(FIFO_1, 0777);
    if(FIFO1to2){
      fprintf(stderr,"Could not create fifo %s\n",FIFO_1);
      exit(EXIT_FAILURE);
    }
  }

  if(access(FIFO_2,F_OK) == -1){
    FIFO2to1 = mkfifo(FIFO_2, 0777);
    if(FIFO2to1){
      fprintf(stderr,"Could not create fifo %s\n",FIFO_2);
      exit(EXIT_FAILURE);
    }
  }

  signal(SIGUSR1,sig_end);
  argv++;

  if(strcmp(*argv,"1") == 0){

    FIFO1to2 = open (FIFO_1,O_WRONLY);
    FIFO2to1 = open (FIFO_2,O_RDONLY);

    pid = fork();
    switch(pid){
      case -1 : perror("Forking failed");  exit(EXIT_FAILURE);
      case  0 : while(strncmp(rbuf,"end chat",8))
                {
                  if((nbytes = read(FIFO2to1,rbuf,MAX_RBUF))>0){
                     printf("User 2 :  \n");
                     write(1,rbuf,nbytes);
                     printf("\n");
                  }
                }
                break;
      default : while(strncmp(rbuf,"end chat",8))
                {
                  if((nbytes = read(0,rbuf,MAX_RBUF))>0){
                    write(FIFO1to2,rbuf,nbytes);
                    printf("-------------------\n");
                  }
                }

    }
  }
  else if(strcmp(*argv,"2")==0){

    FIFO1to2 = open (FIFO_1,O_RDONLY);
    FIFO2to1 = open (FIFO_2,O_WRONLY);

    pid = fork();
    switch(pid){
      case -1 : perror("Forking failed");  exit(EXIT_FAILURE);
      case  0 : while(strncmp(rbuf,"end chat",8))
                {
                  if((nbytes = read(FIFO1to2,rbuf,MAX_RBUF))>0){
                    printf("User 1 :  \n");
                    write(1,rbuf,nbytes);
                    printf("\n");
                  }
                }
                break;
      default : while(strncmp(rbuf,"end chat",8))
                {
                  if((nbytes = read(0,rbuf,MAX_RBUF))>0){
                    write(FIFO2to1,rbuf,nbytes);
                    printf("-------------------\n");
                  }
                }
  } 
}
if(pid > 0) kill(pid,SIGUSR1);
else if(pid == 0) kill(getppid(),SIGUSR1);

if(FIFO1to2 != -1) close(FIFO1to2);
if(FIFO2to1 != -1) close(FIFO2to1);

exit(EXIT_SUCCESS);
}