#include "lexer.h"

#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>

/* Initializes the token array */
void init_tokens(Token tokens[]){
    for(int i = 0 ;i < MAX_TOKENS ; i++){
        tokens[i].type = TOKEN_END ;
        tokens[i].value = NULL ;
    }
}

bool isDelimiter(char chr){
    return (chr == '>' || chr == '<' || chr == '|' || chr == ' ' || chr == '\0') ;
}

bool isOperator(char chr){
    return (chr == '>' || chr == '<' || chr == '|') ;
}

char* getSubstring(char* str , int start, int end){
    int sublength = end - start + 1;
    char* substr = (char*)malloc((sublength+1)*sizeof(char)) ;
    strncpy(substr,str+start,sublength) ;
    substr[sublength] = '\0' ;
    return substr ;
}

void free_tokens(Token tokens[])
{
    for (int i = 0; tokens[i].type != TOKEN_END; i++)
    free(tokens[i].value);
}

void print_tokens(const Token tokens[]){
    int i = 0 ;
    while(tokens[i].type!= TOKEN_END){
        printf("%d  : %s \n",tokens[i].type,tokens[i].value);
        i++;
    }
}

int tokenize(char *input, Token tokens[]){
    init_tokens(tokens) ;

    int n = strlen(input) ;
    int i = 0 ;
    int tokenc = 0 ;

    while(input[i]  && tokenc < MAX_TOKENS - 1){
        if(input[i] == ' '){
            i++ ;
            continue;
        }
        if(isOperator(input[i])){
            if(input[i] == '|'){
                tokens[tokenc].value = getSubstring(input,i,i);
                tokens[tokenc++].type = TOKEN_PIPE ;
            }
            else if(input[i] == '<'){
                tokens[tokenc].value = getSubstring(input,i,i);
                tokens[tokenc++].type = TOKEN_INPUT ;
            }
            else if(input[i] == '>'){
                if(i + 1 < n && input[i+1] == '>'){
                    tokens[tokenc].value = getSubstring(input,i,i+1);
                    tokens[tokenc++].type =  TOKEN_APPEND ;
                    i ++ ;
                }else{
                    tokens[tokenc].value = getSubstring(input,i,i);
                    tokens[tokenc++].type = TOKEN_OUTPUT ;
                }
            }
            i ++ ;        
        }else{
            int k = i ;
            while(input[i]){
                if(isDelimiter(input[i]))   break ;  
                i++ ;
            }
            char *substr = getSubstring(input,k,i-1) ;
            tokens[tokenc].type = TOKEN_WORD ;
            tokens[tokenc++].value = substr ;
        }
        
    }
    tokens[tokenc].value = NULL ;
    tokens[tokenc].type = TOKEN_END ;

    return tokenc ;
}