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
bool wildcard_match(const char *pattern, const char *filename){
    if(*pattern == '\0' && *filename == '\0')   return true ;

    if (*filename == '.' && *pattern != '.') {
        return false;
    }

    if(*pattern == '*'){
        return wildcard_match(pattern + 1,filename) || (*filename != '\0'  && wildcard_match(pattern ,filename +1)) ;
    }

    if(*filename != '\0' && (*pattern == '?' || *pattern == *filename)) {
        return wildcard_match(pattern+1,filename+1);
    }

    return false ;
}

//1
int expand_wildcard(Command *cmd){
    char *new_args[MAX_ARGS] ;
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
                if(wildcard_match(cmd->args[i],entry->d_name)){
                    if(new_argsc < MAX_ARGS -1){
                        new_args[new_argsc ++] = strdup(entry->d_name);
                        match = true ;
                    }
                }
            }
            if(match == false && new_argsc < MAX_ARGS -1){
                new_args[new_argsc ++] = strdup(cmd->args[i]) ;
            }
            closedir(directory) ;
        }else{
            new_args[new_argsc ++] = strdup(cmd->args[i]) ;
        }
    }
    new_args[new_argsc] = NULL ;

    for(int i = 0 ; i < new_argsc ; i ++ ){
        cmd->args[i] = new_args[i] ;
    }

    cmd->args[new_argsc] = NULL ;

    return 0 ;
}