#include "parser.h"
#include "wildcard.h"

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<dirent.h>
#include<string.h>

//2
bool contains_wildcard(const char *str){
    int i = 0 ; 
    while(str[i] != '\0'){
        if(str[i] == '*')       return true ;
        if(str[i] == '?')       return true ;
        i++ ;
    }
    return false ;
}
//3
bool wildcard_match(const char *pattern, const char *filename,bool check){
    if(*pattern == '\0' && *filename == '\0')   return true ;

    if (check && *filename == '.' && *pattern != '.') {
        return false;
    }

    if(*pattern == '*'){
        return wildcard_match(pattern + 1,filename,false) || (*filename != '\0'  && wildcard_match(pattern ,filename +1,false)) ;
    }

    if(*filename != '\0' && (*pattern == '?' || *pattern == *filename)) {
        return wildcard_match(pattern+1,filename+1,false);
    }

    return false ;
}
static void cleanup(char *args[], int count)
{
    for(int i=0;i<count;i++)
        free(args[i]);
}

void free_command_args(Command* cmd){
    for(int i = 0 ; cmd->args[i] != NULL ; i ++){
        free(cmd->args[i]) ;
        cmd->args[i]=NULL;
    }
}
//1
int expand_wildcard(Command *cmd){
    char *new_args[MAX_ARGS] = {NULL} ;
    int new_argsc = 0 ;

    for(int i = 0 ; cmd->args[i] != NULL ; i++){
        if(contains_wildcard(cmd->args[i])){
            struct dirent *entry ;    // pointer for directory entry

            DIR *directory = opendir("."); //returns pointer of DIR type

            if(directory == NULL){
                perror("directory error");
                return -1;
            }
            bool match = false ;

            while((entry = readdir(directory)) != NULL){
                if(wildcard_match(cmd->args[i],entry->d_name,true)){
                    if(new_argsc < MAX_ARGS -1){
                        char *ch = strdup(entry->d_name) ;
                        if(ch == NULL){
                            cleanup(new_args,new_argsc);
                            closedir(directory) ;
                            fprintf(stderr,"strdup error \n") ;
                            return -1 ;
                        }
                        new_args[new_argsc ++] = ch;
                        match = true ;
                    }else{
                        cleanup(new_args,new_argsc);
                        closedir(directory);
                        fprintf(stderr,"wildcard : too many matches\n") ;
                        return -1 ;
                    }
                }
            }
            if(match == false && new_argsc < MAX_ARGS -1){
                char *ch = strdup(cmd->args[i]) ;
                if(ch == NULL){
                    cleanup(new_args,new_argsc);
                    closedir(directory);
                    fprintf(stderr,"strdup error \n") ;
                    return -1 ;
                }
                new_args[new_argsc ++] = ch ;
            }
            closedir(directory) ;
        }else{
            if(new_argsc >= MAX_ARGS -1 ){
                cleanup(new_args,new_argsc);
                fprintf(stderr,"wildcard : too many matches\n") ;
                return -1 ;
            }
            char *ch = strdup(cmd->args[i]) ;
                if(ch == NULL){
                    cleanup(new_args,new_argsc);
                    fprintf(stderr,"strdup error \n") ;
                    return -1 ;
                }
            new_args[new_argsc ++] = ch ;
        }
    }
    new_args[new_argsc] = NULL ;
    free_command_args(cmd);
    for(int i = 0 ; i < new_argsc ; i ++ ){
        cmd->args[i] = new_args[i] ;
    }

    cmd->args[new_argsc] = NULL ;

    return 0 ;
}