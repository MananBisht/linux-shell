#include "builtin.h"
#include "parser.h"

#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

bool is_builtin(const Command *cmd){
    if(cmd->args[0] == NULL )   return false ;
    if(strcmp(cmd->args[0],"cd") == 0)   return true ;
    else if(strcmp(cmd->args[0],"exit") == 0)    return true ;
    return false ;
}

static int builtin_cd(const Command *cmd){
    if(cmd->args[2] != NULL){
            fprintf(stderr,"cd : too many arguments\n");
            return -1 ;
    }
    else if(cmd->args[1] == NULL){
        if(chdir(getenv("HOME"))!= 0){
            perror("chdir error : ");
            return -1 ;
        }
    }else{
        if(chdir(cmd->args[1]) != 0){
            perror("chdir error : ");
            return -1 ;
        }
    }
    
    return 0 ;
}

static int builtin_exit(const Command *cmd){
    (void)cmd ;
    exit(EXIT_SUCCESS) ;
}

int execute_builtin(const Command *cmd){
    if(strcmp(cmd->args[0],"cd") == 0){
        return builtin_cd(cmd) ;
    }else if(strcmp(cmd->args[0],"exit") == 0){
        return builtin_exit(cmd) ;
    }
    return -1;
}

