#include "redirect.h"
#include "parser.h"

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int redirect_output(const Command * cmd ){
    if(cmd->output_file == NULL)   return 0 ;

    int flag ;

    if(cmd->rdir_type == TRUNCATE)
        flag = O_CREAT | O_WRONLY | O_TRUNC ;
    else if(cmd->rdir_type == APPEND)
        flag = O_CREAT | O_WRONLY | O_APPEND ;

    int fd = open(cmd->output_file, flag, 0644) ;
                
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

int redirect_input(const Command * cmd){
    if(cmd->input_file == NULL)   return 0;
    int fd ;
    fd = open(cmd->input_file, O_RDONLY) ;
                
    if(fd == -1 ){
        perror("file error");
        return -1  ;
    }

    if(dup2(fd,STDIN_FILENO) == -1){
        perror("dup2 error");
        close(fd) ;
        return -1 ;                   
    }

    close(fd)  ;
    return 1 ;
}
