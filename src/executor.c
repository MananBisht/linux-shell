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
        waitpid(pid , &status , NULL) ;
    }
    return 0 ;
}


int execute_pipe(const Pipeline *pipeline){
    // for two commands only (e.g. ls | grep txt)
    pid_t pid1,pid2 ;
    int fd[2] ;
    if(pipe(fd) < 0){
        perror("pipe errro : ");
        _exit(EXIT_FAILURE) ;
    }

    pid1 = fork() ;

    if(pid1 == 0 ){
        close(fd[0]) ;
        if(dup2(fd[1],STDOUT_FILENO) < 0 ){
            perror("dup2 error : ");
            close(fd[1]);
            _exit(EXIT_FAILURE) ;
        }
        close(fd[1]) ;

        if(redirect_input(&pipeline->commands[0]) == -1)
            _exit(EXIT_FAILURE);
        if(redirect_output(&pipeline->commands[0]) == -1)
            _exit(EXIT_FAILURE) ;

        if(execvp(pipeline->commands[0].args[0],pipeline->commands[0].args) == -1){
            perror("execvp error : ") ;
            _exit(EXIT_FAILURE);
        }
    }else if( pid1 < 0 ){
        perror("fork error : ") ;
        close(fd[0]) ;
        close(fd[1]) ;
        return -1 ;
    }

    pid2 = fork() ;

    if(pid2 == 0 ){
        close(fd[1]) ;
        if(dup2(fd[0],STDIN_FILENO) < 0 ){
            perror("dup2 error : ");
            close(fd[0]);
            _exit(EXIT_FAILURE) ;
        }
        close(fd[0]) ;

        if(redirect_input(&pipeline->commands[1]) == -1)
            _exit(EXIT_FAILURE);
        if(redirect_output(&pipeline->commands[1]) == -1)
            _exit(EXIT_FAILURE) ;


        if(execvp(pipeline->commands[1].args[0],pipeline->commands[1].args) == -1){
            perror("execvp error : ") ;
            _exit(EXIT_FAILURE);
        } 
    }else if( pid2 < 0 ){
        perror("fork error : ") ;
        close(fd[0]) ;
        close(fd[1]) ;
        return -1 ;
    } 
        close(fd[0]) ;
        close(fd[1]) ;
        int status ;
        waitpid(pid1 , &status , 0) ;
        waitpid(pid2 , &status , 0) ;

        return 0 ;

}

int execute_pipeline(const Pipeline *pipeline){
    if(pipeline->command_count == 0)    return 0 ;
    else if(pipeline->command_count == 1)  return execute_command(&pipeline->commands[0]) ; 
    return execute_pipe(pipeline) ;   
}
