#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<fcntl.h>

enum RedirectType
{
    NONE,
    TRUNCATE,
    APPEND
};


int main(){
    char cmd[1024] = "\0" ;
    char* token ;
    char s[100] ;
    char p_dir[100] ;
    

    getcwd(p_dir,sizeof(p_dir)) ;
    getcwd(s,sizeof(s)) ;

    while(1){
        char* filename = NULL ;
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

        tokenc = 0 ;
        enum RedirectType type = NONE ;

        while(args[tokenc]){
            if(strcmp(args[tokenc],">")==0){
                args[tokenc] = NULL ;
                filename = args[tokenc+1] ;
                type = TRUNCATE ;
                break ;
            } else if(strcmp(args[tokenc],">>")==0){
                args[tokenc] = NULL ;
                filename = args[tokenc + 1] ;
                type = APPEND ;
                break ;
            } 
            tokenc ++ ;
        }

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

        if(strcmp(args[0] ,"exit")==0)   exit(1) ; 


        pid_t pid = fork() ;

        if(pid == 0){

            // implementing basic file descriptor structure

            if(filename!=NULL){
		int fd ;
                if(type == TRUNCATE)
                    fd = open(filename,O_CREAT | O_WRONLY | O_TRUNC, 0644);
                else if(type == APPEND)
                    fd = open(filename,O_CREAT | O_WRONLY | O_APPEND, 0644);
                
                if(fd == -1){
                    perror("file error");
                    exit(1) ;
                }
                
                if(dup2(fd,STDOUT_FILENO) == -1){
                    perror("dup2 error");
                    close(fd) ;
                    exit(1) ;
                }

                close(fd) ;

                if(execvp(args[0],args) == -1){
                    perror("execvp error");
                    exit(1) ;
                }
            }
            if(execvp(args[0],args) == -1 ){
                perror("execvp fail") ;
                exit(1);
            }
        }else if(pid < 0){
            perror("Fork failed") ;
            _exit(EXIT_FAILURE);
        }
        else{
	        int status ;	
            waitpid(pid,&status,0) ;
        }
    }
    return 0 ;
}
