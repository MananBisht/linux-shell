#ifndef PARSER_H
#define PARSER_H

#define MAX_COMMANDS 20
#define MAX_ARGS 100

#include "lexer.h"

enum RedirectType
{
    NONE,
    TRUNCATE,
    APPEND
};

typedef struct{
    char* args[MAX_ARGS] ;
    char* output_file ;
    char* input_file ;
    enum RedirectType rdir_type;

}Command;

typedef struct{
    Command commands[MAX_COMMANDS] ;
    int command_count ;
}Pipeline ;

void init_command(Command *cmd);

void init_pipeline(Pipeline *pipeline);

int parse_pipeline(Token tokens[], Pipeline *pipeline);

void print_pipeline(const Pipeline *pipeline);

#endif