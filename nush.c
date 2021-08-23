#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "list.h"
#include "tokens.h"

void execute(list* tokens);

void
chomp(char* text)
{
    // Removes a new line from the string
    
    assert (strlen(text) >= 1);
    int len = strlen(text);
    if (text[len - 1] == '\n') {
        text[len - 1] = '\0';
    }
}

void
execute_rd_input(list* tokens)
{   
    int cpid;
    char* cmd = tokens->head;
    char* input = NULL;
    
    if ((cpid = fork())) {
        // Parent process
        int status;
        waitpid(cpid, &status, 0);
    }
    else {
        // Child process
        
        // Close STDIN since we are redirecting input
        close(0);
            
        // Account for size - 2 because of < and file path
        int tokens_size = size(tokens) - 2;
            
        // The argv array for the child.
        // Terminated by a null pointer.
        char* args[tokens_size + 1];   // need +1 for NULL at end
        int counter = 0;
        
        // Finding < and the input path which comes directly after <
        int found = 0;
        for (; tokens != NULL; tokens = tokens->tail) {
            if (strcmp(tokens->head, "<") == 0) {
                found = 1;
            }
            else if (found == 1) {
                input = tokens->head;
                found = 0;
            }
            else {
                args[counter] = tokens->head;
                ++counter;
            }
        }
        // Last item in args list needs to be null ptr
        args[counter] = NULL;
        
        // Open file
        FILE* f = fopen(input, "r");
        assert(f != NULL);
        
        // Execute command    
        execvp(cmd, args);
        exit(1);
    }
}

void
execute_rd_output(list* tokens)
{
    int cpid;
    char* cmd = tokens->head;
    char* output = NULL;
    
    if ((cpid = fork())) {
        // Parent process
        int status;
        waitpid(cpid, &status, 0);
    }
    else {
        // Child process
        
        // Close STDOUT since we are redirecting output
        close(1);
            
        // Account for size - 2 because of < and file path
        int tokens_size = size(tokens) - 2;
            
        // The argv array for the child.
        // Terminated by a null pointer.
        char* args[tokens_size + 1];   // need +1 for NULL at end
        int counter = 0;
        
        // Finding > and the input path which comes directly after >
        int found = 0;
        for (; tokens != NULL; tokens = tokens->tail) {
            if (strcmp(tokens->head, ">") == 0) {
                found = 1;
            }
            else if (found == 1) {
                output = tokens->head;
                found = 0;
            }
            else {
                args[counter] = tokens->head;
                ++counter;
            }
        }
        // Last argument in args needs to be null ptr
        args[counter] = NULL;
        
        // Open output file
        FILE* f = fopen(output, "w");
        assert(f != NULL);
            
        execvp(cmd, args);
        exit(1);
    }
}

pid_t
execute_pipe_before(list* tokens, int pipe_fds[2]) 
{   
    pid_t pid = fork();
    char* cmd = tokens->head;
    
    if (pid > 0) {
            // Parent process
            // We don't need to wait for the child process to finish here
            return pid;
        }
        else {
            
            // Close STDOUT and the pipe end that we are not using
            close(pipe_fds[0]);
            close(1);
            assert(dup(pipe_fds[1]) == 1);
            
            int tokens_size = size(tokens);
            
            // The argv array for the child.
            // Terminated by a null pointer.
            char* args[tokens_size + 1];   // need +1 for NULL at end
            int counter = 0;
            
            // Putting the tokens into the args array
            for (; tokens != NULL; tokens = tokens->tail) {
                // Making sure the token is null terminated
                tokens->head[strlen(tokens->head) + 1] = '\0';
                args[counter] = tokens->head;
                ++counter;
            }
            // Last item in args must be null ptr
            args[counter] = NULL;
            
            execvp(cmd, args);
            exit(1);
        } 
}

pid_t
execute_pipe_after(list* tokens, int pipe_fds[2])
{  
    pid_t pid = fork();
    char* cmd = tokens->head;
    
    if (pid > 0) {
        // Parent process
        // We don't need to wait for the child process to end
        
        return pid;
    }
    else {
        // Child process
        
        // Close STDIN and the pipe end that we are not using
        close(pipe_fds[1]);
        close(0);
        assert(dup(pipe_fds[0]) == 0);
            
        int tokens_size = size(tokens);
            
        // The argv array for the child.
        // Terminated by a null pointer.
        char* args[tokens_size + 1];   // need +1 for NULL at end
        int counter = 0;
            
        // Putting tokens into args array
        for (; tokens != NULL; tokens = tokens->tail) {
                args[counter] = tokens->head;
                ++counter;
        }
        // Last item in args needs to be null ptr
        args[counter] = NULL;
            
        execvp(cmd, args);
        exit(1);
    } 
}

void
execute_pipe(list* tokens)
{
    // Setup our pipe
    int pipe_fds[2];
    assert(pipe(pipe_fds) == 0);

    list* rev_before = NULL;
    list* rev_after = NULL;
    
    // Find before and after pipe
    int found = 0;
    for (; tokens != NULL; tokens = tokens->tail) {
        if (strcmp(tokens->head, "|") == 0) {
            found = 1;
            continue;
        }
        if (found == 0) {
            rev_before = cons(tokens->head, rev_before);
        }
        else if (found == 1) {
            rev_after = cons(tokens->head, rev_after);
        }
    }
    
    list* before = reverse(rev_before);
    list* after = reverse(rev_after);
    
    pid_t before_pid = execute_pipe_before(before, pipe_fds);
    pid_t after_pid = execute_pipe_after(after, pipe_fds);
    
    // Close pipes once child processes have finished reading/writing
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    
    // Waiting for child processes to finish
    waitpid(before_pid, NULL, 0);
    waitpid(after_pid, NULL, 0);
}

void
execute_background(list* tokens)
{    
    int cpid;
    if ((cpid = fork())) {
    // Parent process
    // Don't need to do anything, child process will run in background and finish when it's done
    }
    else {
        // Child process
            
        int tokens_size = size(tokens);
        char* cmd = tokens->head;
            
        // The argv array for the child.
        // Terminated by a null pointer.
        char* args[tokens_size + 1];   // need +1 for NULL at end
        int counter = 0;
            
        // Putting the tokens into args array
        for (; tokens != NULL; tokens = tokens->tail) {
            args[counter] = tokens->head;
            ++counter;
        }
        // Last item in args array must be null ptr
        args[counter] = NULL;
        
        execvp(cmd, args);
        exit(1);
    }
}

void
execute_and(list* tokens)
{    
    list* rev_before = NULL;
    list* rev_after = NULL;
    
    // Finding && and the commands before and after &&
    int found = 0;
    for (; tokens; tokens = tokens->tail) {
        if (strcmp(tokens->head, "&&") == 0) {
            found = 1;
            continue;
        }
        if (found == 0) {
            rev_before = cons(tokens->head, rev_before);
        }
        if (found == 1) {
            rev_after = cons(tokens->head, rev_after);
        }
    }
    
    list* before = reverse(rev_before);
    list* after = reverse(rev_after);
    
    char* before_cmd = before->head;
    
    int cpid;
    if ((cpid = fork())) {
            // Parent process
        
            int status;
            waitpid(cpid, &status, 0);

            // Check for exit and exit status
            if (WIFEXITED(status)) {
                if (!WEXITSTATUS(status)) {                   
                    execute(after);
                    // free_list(rev_before);
                    // free_list(rev_after);
                    // free_list(before);
                    // free_list(after);                    
                }
            }
    }
    else {
        // Child process
            
        int tokens_size = size(before);
            
        // The argv array for the child.
        // Terminated by a null pointer.
        char* args[tokens_size + 1];   // need +1 for NULL at end
        int counter = 0;
            
        for (; before != NULL; before = before->tail) {
                args[counter] = before->head;
                ++counter;
        }
        // Last item in args array needs to be null ptr
        args[counter] = NULL;
            
        execvp(before_cmd, args);
        exit(-1);
    } 
}

void
execute_or(list* tokens)
{
    list* rev_before = NULL;
    list* rev_after = NULL;
    
    // Finding || and the command before and after the ||
    int found = 0;
    for (; tokens; tokens = tokens->tail) {
        if (strcmp(tokens->head, "||") == 0) {
            found = 1;
            continue;
        }
        if (found == 0) {
            rev_before = cons(tokens->head, rev_before);
        }
        if (found == 1) {
            rev_after = cons(tokens->head, rev_after);
        }
    }
    
    list* before = reverse(rev_before);
    list* after = reverse(rev_after);
    
    char* before_cmd = before->head;
    
    int cpid;
    if ((cpid = fork())) {
        // Parent process

        int status;
        waitpid(cpid, &status, 0);

        // Check for exit and exit status
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status)) {                                        
                execute(after);
                // free_list(rev_before);
                // free_list(rev_after);
                // free_list(before);
                // free_list(after);                    
            }
        }
    }
    else {
        // Child process
            
        int tokens_size = size(before);
            
        // The argv array for the child.
        // Terminated by a null pointer.
        char* args[tokens_size + 1];   // need +1 for NULL at end
        int counter = 0;
            
        // Putting tokens into args array
        for (; before != NULL; before = before->tail) {
            args[counter] = before->head;
            ++counter;
        }
        // Last item in args array must be null ptr
        args[counter] = NULL;
            
        execvp(before_cmd, args);
        exit(1);
    } 
}

void
execute_semicolon(list* tokens)
{
    list* rev_before = NULL;
    list* rev_after = NULL;
    
    // Finding ; and the commands before and after the ;
    int found = 0;
    for (; tokens; tokens = tokens->tail) {
        if (strcmp(tokens->head, ";") == 0) {
            found = 1;
            continue;
        }
        if (found == 0) {
            rev_before = cons(tokens->head, rev_before);
        }
        if (found == 1) {
            rev_after = cons(tokens->head, rev_after);
        }
    }
    
    list* before = reverse(rev_before);
    list* after = reverse(rev_after);
    
    execute(before);
    execute(after);
    // free_list(rev_before);
    // free_list(before);
    // free_list(rev_after);
    // free_list(after);    
}

void
execute(list* tokens)
{     
    int cpid;
    char* cmd = tokens->head;
    
    // 1 if the operator is present, 0 otherwise
    int has_rd_input = has("<", tokens);
    int has_rd_output = has(">", tokens);
    int has_pipe = has("|", tokens);
    int has_background = has("&", tokens);
    int has_and = has("&&", tokens);
    int has_or = has("||", tokens);
    int has_semicolon = has(";", tokens);
    
    // check for exit
    if (strcmp(tokens->head, "exit") == 0) {
        exit(0);
    }
    // check for pwd
    else if (strcmp(tokens->head, "pwd") == 0) {
        char cwd[256];
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        fflush(stdout);
        return;
    }
    // check for cd
    else if (strcmp(tokens->head, "cd") == 0) {
        list* dir = tokens->tail;
        chdir(dir->head);
        fflush(stdout);
        return;
    }
    
    int precendence_found = 0;
    
    // Below if statements check for operators in the command and call the correct execute function
    if (has_semicolon == 1 && precendence_found == 0) {
        precendence_found = 1;
        execute_semicolon(tokens);
    }
    if (has_rd_input == 1 && precendence_found == 0) {
        precendence_found = 1;
        execute_rd_input(tokens);
    }
    if (has_rd_output == 1 && precendence_found == 0) {
        precendence_found = 1;
        execute_rd_output(tokens);
    }
    if (has_pipe == 1 && precendence_found == 0) {
        precendence_found = 1;
        execute_pipe(tokens);
    }
    if (has_and == 1 && precendence_found == 0) {
        precendence_found = 1;
        execute_and(tokens);
    }
    if (has_or == 1 && precendence_found == 0) {
        precendence_found = 1;
        execute_or(tokens);
    }
    if (has_background == 1 && precendence_found == 0) {
        precendence_found = 1;
        execute_background(tokens);
    }
    // None of the above operators are used, so we just execute the normal command
    if (precendence_found == 0) {
        if ((cpid = fork())) {
            // Parent process

            int status;
            waitpid(cpid, &status, 0);
        }
        else {
            // Child process
            
            int tokens_size = size(tokens);
            
            // The argv array for the child.
            // Terminated by a null pointer.
            char* args[tokens_size + 1];   // need +1 for NULL at end
            int counter = 0;
            
            // Putting tokens into args array
            for (; tokens != NULL; tokens = tokens->tail) {
                args[counter] = tokens->head;
                ++counter;
            }
            // Last item in args array must be null ptr
            args[counter] = NULL;
            
            execvp(cmd, args);
            exit(1);
        }
    }
}

int
main(int argc, char* argv[])
{
    char cmd[256];
    
    // No optional script provided
    if (argc == 1) {
        printf("nush$ ");
        fflush(stdout);
        while(fgets(cmd, 256, stdin) != NULL) {
            chomp(cmd);
            list* rev_tokens = NULL;
            list* tokens = NULL;
            rev_tokens = tokenize(cmd);
            tokens = reverse(rev_tokens);
            execute(tokens);
            printf("nush$ ");
            fflush(stdout);
            // free_list(rev_tokens);
            // free_list(tokens);
        }
    }
    // Optional script provided - run the script
    else {
        FILE* f;
        f = fopen(argv[1], "r");
        while(fgets(cmd, 256, f) != NULL) {
            // printf("printing command from file: %s\n", cmd);
            chomp(cmd);
            list* rev_tokens = NULL;
            list* tokens = NULL;
            rev_tokens = tokenize(cmd);
            tokens = reverse(rev_tokens);
            execute(tokens);
            // free_list(tokens);
            // free_list(rev_tokens);
        }
        fclose(f);
    }

    return 0;
}
