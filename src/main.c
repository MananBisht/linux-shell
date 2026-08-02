#include "redirect.h"
#include "lexer.h"
#include "parser.h"
#include "executor.h"
#include "builtin.h"
#include "wildcard.h"

#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<fcntl.h>

int main(){
    Token tokens[MAX_TOKENS] ;
    Pipeline pipeline ;
    char cmd[1024] = "\0" ;

    char s[100] ;
    char p_dir[100] ;
    
    getcwd(p_dir,sizeof(p_dir)) ;
    getcwd(s,sizeof(s)) ;

    while(1){
        // core initialization
        init_tokens(tokens) ;
        init_pipeline(&pipeline);

        //prompt
        if(strcmp(s,p_dir)==0)
            printf("myshell> ") ;
        else{
            printf("myshell>-[%s] ",s) ;
        }

        if(fgets(cmd,sizeof(cmd),stdin) == NULL)   break ;
        cmd[strcspn(cmd,"\n")] = '\0' ;

        // tokenization
        if(tokenize(cmd ,tokens) == -1){
            free_tokens(tokens) ;
            continue ;
        }
        
        // parsing
        if(parse_pipeline(tokens,&pipeline) == -1){
            free_tokens(tokens) ;
            continue ;
        }
        
        // empty command
        if(pipeline.command_count == 0){     
            free_tokens(tokens) ;
            continue ;
        }
        
        // wildcards
        
        int error = 0 ;
        for(int i = 0 ; i < pipeline.command_count ; i++){
            if(expand_wildcard(&pipeline.commands[i]) == -1 ){
                error = 1 ;
                break ;
            }
        }
        if(error == 1){
            free_tokens(tokens) ;
            free_pipeline(&pipeline) ;
            continue;
        }
        // single and builtin command execution
        if(pipeline.command_count == 1 && is_builtin(&pipeline.commands[0])){
                execute_builtin(&pipeline.commands[0]) ;
                getcwd(s, sizeof(s));
        }
        // command execution
        else{
            execute_pipeline(&pipeline) ;
        }

        free_tokens(tokens);
        
    }
    return 0 ;
}
