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

typedef struct
{
    char* output_file ;
    char* input_file ;
    enum RedirectType rdir_type;
} RedirectInfo ;

int redirect_output(const RedirectInfo *redir){
    if(redir->output_file == NULL)   return 0 ;

    int fd ;
    if(redir->rdir_type == TRUNCATE)
        fd = open(redir->output_file,O_CREAT | O_WRONLY | O_TRUNC, 0644);
    else if(redir->rdir_type == APPEND)
        fd = open(redir->output_file,O_CREAT | O_WRONLY | O_APPEND, 0644);
                
    if(fd == -1 ){
        perror("file error");
        return -1 ;
    }

    if(dup2(fd,STDOUT_FILENO) == -1){
        perror("dup2 error");
        close(fd) ;
        return -1 ;
    }

    close(fd)  ;
    return 1 ;
}

int redirect_input(const RedirectInfo *redir){
    if(redir->input_file == NULL)   return 0;
    int fd ;
    fd = open(redir->input_file, O_RDONLY) ;
                
    if(fd == -1 ){
        perror("file error");
        exit(1) ;
    }

    if(dup2(fd,STDIN_FILENO) == -1){
        perror("dup2 error");
        close(fd) ;
        return -1 ;                   
    }

    close(fd)  ;
    return 1 ;
}


int main(){
    char cmd[1024] = "\0" ;
    char* token ;
    char s[100] ;
    char p_dir[100] ;
    

    getcwd(p_dir,sizeof(p_dir)) ;
    getcwd(s,sizeof(s)) ;

    while(1){
        RedirectInfo redir ;
        redir.input_file = NULL ;
        redir.output_file = NULL ;
        redir.rdir_type = NONE ;
        

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
        int error = 0 ;

        while(args[tokenc]){
            if(strcmp(args[tokenc],">")==0){
                if(args[tokenc+1] == NULL){
                    printf("error :no file\n") ;
                    error = 1 ;
                    break ;
                }
                args[tokenc] = NULL ;
                redir.output_file = args[tokenc+1] ;
                redir.rdir_type = TRUNCATE ;
            } else if(strcmp(args[tokenc],">>")==0){
                if(args[tokenc+1] == NULL){
                    printf("error :no file\n") ;
                    error = 1 ;
                    break ;
                }
                args[tokenc] = NULL ;
                redir.output_file = args[tokenc + 1] ;
                redir.rdir_type = APPEND ;
            } else if(strcmp(args[tokenc],"<") == 0){
                if(args[tokenc+1] == NULL){
                    printf("error :no file\n") ;
                    error = 1 ;
                    break ;
                }
                args[tokenc] = NULL ;
                redir.input_file = args[tokenc+1] ;
            }
            tokenc ++ ;
        }

        if(error == 1 )     continue ;

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

        if(strcmp(args[0] ,"exit")==0)   break ;; 


        pid_t pid = fork() ;

        if(pid == 0){

            // implementing file descriptor

            // if (redirect_output(&redir) == -1)
            //     _exit(EXIT_FAILURE); 
            
            // if (redirect_input(&redir) == -1)
            //     _exit(EXIT_FAILURE); 

            redirect_output(&redir);
            redirect_input(&redir);

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
