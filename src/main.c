#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>

int main(){
    char cmd[1024] = "\0" ;
    char* token ;

    while(1){
        printf("myshell> ") ;

        if(fgets(cmd,sizeof(cmd),stdin) == NULL)   break ;
        cmd[strcspn(cmd,"\n")] = '\0' ;

        token = strtok(cmd," ") ;
        if(token == NULL)    continue ;

        int tokenc = 0 ;
        char* args[100] ;

        while(token != NULL){
            args[tokenc] = token ;
            tokenc ++ ;
            token = strtok(NULL, " ");
        }
        args[tokenc] = NULL ;

        if(strcmp(cmd ,"exit")==0)   exit(1) ; 

        pid_t pid = fork() ;

        if(pid == 0){
            if(execvp(args[0],args) == -1 ){
                perror("execvp fail") ;
                exit(1);
            }
        }else if(pid < 0){
            perror("Fork failed") ;
            exit(1) ;
        }
        else{
	        int status ;	
            waitpid(pid,&status,0) ;
        }
    }
    return 0 ;
}
