#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

int execute_pipeline(const Pipeline *pipeline);

int execute_command(const Command *cmd);


int execute_pipe(const Pipeline *pipeline);

#endif