#include "parser.h"
#include "lexer.h"

#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

void init_command(Command *cmd){
    cmd->input_file = NULL ;
    cmd->output_file = NULL ;
    cmd->rdir_type = NONE ;

    for(int i = 0 ; i < MAX_ARGS ; i ++ ){
        cmd->args[i] = NULL ;
    }
}

void init_pipeline(Pipeline *pipeline){
    for(int i = 0 ; i < MAX_COMMANDS ; i ++)
    init_command(&pipeline->commands[i]);
    pipeline->command_count = 0 ;
}

void free_command(Command *cmd){
    for(int i = 0; cmd->args[i] != NULL ; i++){
        free(cmd->args[i]);
    }
    free(cmd->input_file);
    free(cmd->output_file) ;
    init_command(cmd) ;
}

void free_pipeline(Pipeline *pipeline){
    for(int i = 0; i< pipeline->command_count ; i++){
        free_command(&pipeline->commands[i]);
    }
    pipeline->command_count = 0;
}

int parse_pipeline(Token tokens[], Pipeline *pipeline){
    init_pipeline(pipeline) ;

    int current_token = 0 ;
    int command_count = 0 ;
    int current_args = 0 ;

    while(tokens[current_token].type != TOKEN_END){
        switch (tokens[current_token].type){
            case TOKEN_WORD:
                if (current_args >= MAX_ARGS - 1) { 
                    return -1;
                }
                char * ch = strdup(tokens[current_token++].value) ;
                if(ch == NULL){
                    fprintf(stderr,"strdup error \n") ;
                    free_pipeline(pipeline);
                    return -1 ;
                }

                pipeline->commands[command_count].args[current_args++] = ch ;
                break;
            case TOKEN_INPUT:
                if(tokens[current_token + 1].type == TOKEN_WORD ){
                    char * ch = strdup(tokens[current_token+1].value) ;
                    if(ch == NULL){
                        free_pipeline(pipeline);
                        fprintf(stderr,"strdup error \n") ;
                        return -1 ;
                    }
                    pipeline->commands[command_count].input_file = ch ;
                    current_token += 2 ;
                }else   
                    return -1 ;
                break ;
            case TOKEN_OUTPUT :
                if(tokens[current_token + 1].type == TOKEN_WORD ){
                    char * ch = strdup(tokens[current_token+1].value) ;
                if(ch == NULL){
                    free_pipeline(pipeline);
                    fprintf(stderr,"strdup error \n") ;
                    return -1 ;
                }
                    pipeline->commands[command_count].output_file = ch ;
                    pipeline->commands[command_count].rdir_type = TRUNCATE ;
                    current_token += 2 ;
                }else
                    return -1 ;
                break ;
            case TOKEN_APPEND :
                if(tokens[current_token + 1].type == TOKEN_WORD ){
                    char * ch = strdup(tokens[current_token+1].value) ;
                if(ch == NULL){
                    free_pipeline(pipeline);
                    fprintf(stderr,"strdup error \n") ;
                    return -1 ;
                }
                    pipeline->commands[command_count].output_file = ch;
                    pipeline->commands[command_count].rdir_type = APPEND ;
                    current_token += 2 ;
                }else
                    return -1 ;
                break ;
            case TOKEN_PIPE :
                if( command_count >= MAX_COMMANDS - 1)      return -1 ;
                if(tokens[0].type == TOKEN_PIPE || tokens[current_token + 1].type != TOKEN_WORD)
                    return -1 ;
                pipeline->commands[command_count].args[current_args] = NULL;
                current_args = 0 ;
                command_count ++ ;
                current_token ++ ;
                break ;
            case TOKEN_END :
                break ;
        }
    }
    if(command_count +1 > MAX_COMMANDS){
        free_pipeline(pipeline) ;
        return -1 ;
    }
    pipeline->commands[command_count].args[current_args] = NULL;
    pipeline->command_count = command_count + 1 ;
    if(tokens[0].type == TOKEN_END)
        pipeline->command_count = 0 ;

    return 0 ;
}

void print_pipeline(const Pipeline *pipeline) {
    printf("%d command:\n", pipeline->command_count);

    for (int i = 0; i < pipeline->command_count; i++) {
        const Command *cmd = &pipeline->commands[i];
        printf("  Command %d:\n", i + 1);

        printf("    Arguments: ");
        for (int j = 0; cmd->args[j] != NULL; j++) {
            printf("%s ", cmd->args[j]);
        }
        printf("\n");

        if (cmd->input_file != NULL) {
            printf("    Input file: %s\n", cmd->input_file);
        }
        if (cmd->output_file != NULL) {
            printf("    Output file: %s\n", cmd->output_file);
            if (cmd->rdir_type == TRUNCATE) {
                printf("    Redirect type: TRUNCATE\n");
            } else if (cmd->rdir_type == APPEND) {
                printf("    Redirect type: APPEND\n");
            }
        }
    }
}
