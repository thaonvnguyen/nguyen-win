/*
* Name: Thao Nguyen
* ID #: 1001142617
* Programming Assignment 1
* Description:
*/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_COMPONENT 7 //command and 5 arguments, and a terminating NULL

void tokenize(char * s, char * args[MAX_COMPONENT]){
  int i =0;
  char *p = strtok (s, " ");
  while (i<MAX_COMPONENT){
    if(p != NULL){
      args[i++] = p;
      p = strtok (NULL, " ");
    }if (p==NULL)
      args[i++]= NULL;
  }
}

/*Decapitalize a string*/
void toLowerCase(char* s){
if (s!=NULL){
for ( ; *s; ++s) 
  *s = tolower(*s);
}
}

char * concat(char* s1, char * s2 ){
  char *result = malloc(strlen(s1)+strlen(s2)+1);
  strcpy(result,s1);
  strcat(result, s2);
  return result;
}

/*
* Input: a string as a command
* Go to the appropriate location and execute the command
*/
void execute(char **cmd){
  toLowerCase(cmd[0]);
  if(strcmp(cmd[0],"exit")==0 || strcmp(cmd[0],"quit")==0)
    exit(0);

  int status=0;
  char * binpath = concat("/bin/", cmd[0]);
  char * usrbinpath = concat("/usr/bin/", cmd[0]);
  char * usrlocalbinpath= concat("/usr/local/bin/", cmd[0]);

  pid_t child_pid = fork();
  if( child_pid == 0 ){
    status = execl(binpath, cmd[0], cmd[1],cmd[2], cmd[3], cmd[4],cmd[5], NULL);
    if( status == -1 ){
      status = execl(usrbinpath, cmd[0], cmd[1],cmd[2], cmd[3], cmd[4],cmd[5], NULL );
      }
    if( status == -1 ){
      status = execl(usrbinpath, cmd[0], cmd[1],cmd[2], cmd[3], cmd[4],cmd[5], NULL );
    }
  }
  if(status == -1)
      printf("%s : Command not found.\n",cmd[0]);
  waitpid(child_pid, &status,0);

}

int main(void){
  char *line;
  char * name;
  long unsigned int len;
  while(1){
    char * cmd[MAX_COMPONENT];
    printf("msh> ");
    getline(&line, &len, stdin);
    int i;
  
    //trim the command line down to its actual size
    for(i=0;i<len;i++){
      if(line[i]=='\n'){
        line[i]='\0';
        len = i;
        break;
      }
    }
      tokenize(line, cmd);
      if(cmd[0]!=NULL)
        execute(cmd);
}
}

