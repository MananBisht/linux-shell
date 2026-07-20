#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>

int main(){
    char cmd[1024] = "\0" ;
    char* token ;
    char s[100] ;

    char p_dir[100] ;
    getcwd(p_dir,sizeof(p_dir)) ;
    getcwd(s,sizeof(s)) ;

    while(1){
        if(strcmp(s,p_dir)==0)
            printf("myshell> ") ;
        else{
            printf("myshell>-[%s] ",s) ;
        }

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

        // implementing cd command

        if(strcmp(args[0],"cd")==0){
            if(args[1]!=NULL){
                if(chdir(args[1])!= 0)
                    perror("chdir error");
            }else{
                chdir(getenv("HOME")) ;
            }
            getcwd(s,sizeof(s)) ;
            continue ;
        }

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
