#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>
#include <ctype.h>

#include "list.h"

void
append(char* s, char c) 
{
    // printf("appending char to token\n");

    int len = 0;
    
    if (s != NULL) {
        len = strlen(s);
        s[len] = c;
        s[len + 1] = '\0';
    }
    else {
        s = "a";
        strlcpy(s, &c, 1);
    }
}

list*
tokenize(const char* command)
{
    // printf("%s", command);
    int len = strlen(command);
    char* token = malloc(sizeof(char));
    list* new_tokens = NULL;
    
    for (int i = 0; i < len + 1; ++i) {
        if (i == len) {
            if (token != NULL) {
                // printf("last token: %s\n", token);
                char* new_token = strdup(token);
                new_token[strlen(new_token) + 1] = '\0';
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
            }
        }
        if (command[i] == '\0') {
            continue;
        }
        if (token == NULL) {
            token = malloc(sizeof(char));
        }        
        if (isspace(command[i])) {
            // printf("found space\n");
            if (strlen(token) > 0) {
                // printf("adding token to list of tokens\n");
                // printf("this is the token: %s\n", token);
                char * new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
                
            }
            continue;
        }
        if (command[i] == '<') {
            if (strlen(token) > 0) {
                char* new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
                token = malloc(sizeof(char));
                append(token, command[i]);
                new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
                continue;
            }
            else {
                append(token, command[i]);
                char* new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
                continue;
            }
        }
        if (command[i] == '>') {
            if (strlen(token) > 0) {
                char* new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
                token = malloc(sizeof(char));
                append(token, command[i]);
                new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
                continue;
            }
            else {
                append(token, command[i]);
                char* new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
                continue;
            }
        }
        if (command[i] == '|') {
            int or = 0;
            if (command[i+1] == '|') {
                or = 1;
            }
            if (or == 1) {
                if (strlen(token) > 0) {
                    char* new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    token = malloc(sizeof(char));
                    append(token, command[i]);
                    append(token, command[i+1]);
                    new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    ++i;
                    continue;
                }
                else {
                    append(token, command[i]);
                    append(token, command[i+1]);
                    char* new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    ++i;
                    continue;
                }
            }
            else {
                if (strlen(token) > 0) {
                    char* new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    token = malloc(sizeof(char));
                    append(token, command[i]);
                    new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    continue;
                }
                else {
                    append(token, command[i]);
                    char* new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    continue;
                }
            }
        }
        if (command[i] == '&') {
            int and = 0;
            if (command[i+1] == '&') {
                and = 1;
            }
            if (and == 1) {
                if (strlen(token) > 0) {
                    char * new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    token = malloc(sizeof(char));
                    append(token, command[i]);
                    append(token, command[i+1]);
                    new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    ++i;
                    continue;
                }
                else {
                    append(token, command[i]);
                    append(token, command[i+1]);
                    char * new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    ++i;
                    continue;
                }
            }
            else {
                if (strlen(token) > 0) {
                    char * new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    token = malloc(sizeof(char));
                    append(token, command[i]);
                    new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    continue;
                }
                else {
                    append(token, command[i]);
                    char * new_token = strdup(token);
                    free(token);
                    token = NULL;
                    new_tokens = cons(new_token, new_tokens);
                    continue;
                }
            }
        }
        if (command[i] == ';') {
            if (strlen(token) > 0) {
                char * new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
                token = malloc(sizeof(char));
                append(token, command[i]);
                new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
            }
            else {
                append(token, command[i]);
                char * new_token = strdup(token);
                free(token);
                token = NULL;
                new_tokens = cons(new_token, new_tokens);
            }
        }
        else {
            // printf("not space, appending char\n");
            append(token, command[i]);
        }
    }
    // printf("**printing list of tokens from tokenize:\n");
    // print_list(new_tokens);
    free(token);
    return(new_tokens);
}

/*
int
main(int argc, char* argv[])
{
    char line[100];
    list* tokens;
    
   while (fgets(line, 96, stdin) != NULL) {
       // printf("%s", line);
       tokens = tokenize(line);
       // List is already in reverse order
       for (; tokens; tokens = tokens->tail) {
           printf("%s\n", tokens->head);
       }
   }
   // printf("out of loop successfully\n");
      
    return 0;
}
*/




