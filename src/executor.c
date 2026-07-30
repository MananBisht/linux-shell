#include "executor.h"
#include "parser.h"
#include "redirect.h"

#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>

int execute_command(const Command *cmd){
    pid_t pid = fork() ;

    if(pid == 0){

        if(redirect_input(cmd) == -1)
            _exit(EXIT_FAILURE);
        
        if(redirect_output(cmd) == -1 )
            _exit(EXIT_FAILURE);
        
        if(execvp(cmd->args[0],cmd->args) == -1){
            perror("execvp error : ") ;
            _exit(EXIT_FAILURE);
        }
    }else if( pid < 0 ){
        perror("fork error : ") ;
        return -1 ;
    }else{
        int status ;
        waitpid(pid , &status , 0) ;
    }
    return 1 ;
}

// N command pipe execution 
int execute_pipe(const Pipeline *pipeline){
    int prev_read = -1 ;
    int command = pipeline->command_count ;
    pid_t pids[command] ;

    for(int i = 0 ; i < command ; i ++){
        int fd[2] = {-1,-1} ;

        if(i != command-1 ){
            if(pipe(fd) == -1){
                perror("pipe error ") ;
                return -1  ;
            }
        }

        pid_t pid = fork() ;

        if(pid == -1 ){
            perror("fork error ") ;
            return -1 ;
        }

        if(pid == 0){
            if(i > 0){
                if(dup2(prev_read,STDIN_FILENO) == -1){
                    perror("dup2 error ") ;
                    _exit(EXIT_FAILURE);
                }
            }
            if( i < command -1){
                if(dup2(fd[1],STDOUT_FILENO) == -1){
                    perror("dup2 error ") ;
                    _exit(EXIT_FAILURE);
                }
            }

            if(fd[1] != -1)     close(fd[1]) ;
            if(fd[0] != -1)     close(fd[0]) ;
            if(prev_read != -1 )close(prev_read ) ;

            if(redirect_input(&pipeline->commands[i] )== -1)
                _exit(EXIT_FAILURE) ;
            if(redirect_output(&pipeline->commands[i] ) == -1 )
                _exit(EXIT_FAILURE) ;
            
            if(execvp(pipeline->commands[i].args[0],pipeline->commands[i].args) == -1){
                perror("execvp error ");
                _exit(EXIT_FAILURE);
            }
        }

        pids[i] = pid ;

        if(prev_read != -1)        close(prev_read) ;

        if( i < command -1 ){
            prev_read = fd[0] ;
            close(fd[1]) ;
        }
    }
    if(prev_read != -1)   close(prev_read) ;  
    int status;

    for(int i = 0 ; i < command ; i ++ )
        waitpid(pids[i] , &status, 0 ) ;
    
    return 0 ;
}

int execute_pipeline(const Pipeline *pipeline){
    if(pipeline->command_count == 0)    return 0 ;
    else if(pipeline->command_count == 1)  return execute_command(&pipeline->commands[0]) ; 
    return execute_pipe(pipeline) ;   
}
