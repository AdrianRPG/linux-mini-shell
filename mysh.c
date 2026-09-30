/*
 * mysh.c - a mini-shell
 *
 * COP 4610 Operating Systems Principles, Fall 2026
 * Homework 1   (authors: see README.md)
 *
 * The main loop and the tokenizer are written for you.
 * Everything marked TODO is yours. The Module 1 slide pages named next
 * to each TODO contain the mechanism that function needs.
 */

#include <stdio.h>      /* printf, fgets, perror                          */
#include <stdlib.h>     /* exit, getenv                                   */
#include <string.h>     /* strcmp, strtok, strcspn, strchr                */
#include <unistd.h>     /* fork, execvp, chdir, getcwd, dup2, pipe, close */
#include <sys/wait.h>   /* wait, waitpid                                  */
#include <fcntl.h>      /* open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_APPEND */

#define MAX_INPUT 1024  /* longest command line we accept          */
#define MAX_ARGS    64  /* most words on one line, including NULL  */

void parse_input(char *input, char **args, int *argc);
int  is_builtin(char **args);
void execute_builtin(char **args);
void execute_command(char **args);
int  has_redirection(char **args);
void execute_redirection(char **args);
int  has_pipe(char *input);
void execute_pipe(char *input);

/* ------------------------------------------------------------------ */
/* main: read a line, decide what kind of line it is, act, repeat     */
/* (slides: "The Shell Loop")                                        */
/* ------------------------------------------------------------------ */
int main(void)
{
    char  input[MAX_INPUT];
    char *args[MAX_ARGS];
    int   argc;

    while (1) {
        printf("mysh> ");
        fflush(stdout);                 /* the prompt has no newline; push it out */

        if (fgets(input, sizeof input, stdin) == NULL) {
            printf("\n");               /* Ctrl+D: end of input, leave quietly */
            break;
        }
        input[strcspn(input, "\n")] = '\0';   /* drop the trailing newline */
        if (input[0] == '\0')
            continue;                   /* empty line: just show the prompt again */

        if (has_pipe(input)) {          /* "cmd1 | cmd2" is split inside execute_pipe */
            execute_pipe(input);
            continue;
        }

        parse_input(input, args, &argc);
        if (argc == 0)
            continue;

        if (is_builtin(args))
            execute_builtin(args);      /* runs inside the shell itself: no fork */
        else if (has_redirection(args))
            execute_redirection(args);
        else
            execute_command(args);
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* parse_input: split a line into words.  Given.                       */
/*                                                                     */
/*   "ls -l -a"  ->  args[0]="ls" args[1]="-l" args[2]="-a" args[3]=NULL*/
/*                                                                     */
/* strtok writes '\0' into `input` and returns pointers into it, so    */
/* the words live inside `input` -- do not reuse `input` while `args`  */
/* is still needed.  The array is NULL-terminated because execvp       */
/* needs that to know where the arguments stop.                        */
/* ------------------------------------------------------------------ */
void parse_input(char *input, char **args, int *argc)
{
    *argc = 0;
    char *token = strtok(input, " ");
    while (token != NULL && *argc < MAX_ARGS - 1) {
        args[(*argc)++] = token;
        token = strtok(NULL, " ");
    }
    args[*argc] = NULL;
}

/* ------------------------------------------------------------------ */
/* Built-ins: cd, pwd, exit                                            */
/* (slides: "Separation of fork() and exec()", the Built-ins note)    */
/* ------------------------------------------------------------------ */
int is_builtin(char **args)
{
	if (strcmp(args[0], "cd") == 0 ||
            strcmp(args[0], "pwd") == 0 ||
            strcmp(args[0], "exit") == 0) {
		return 1;
	}
    return 0;
}

void execute_builtin(char **args)
{
	if (strcmp(args[0], "cd") == 0) {
		char *dir;

		if(args[1] != NULL)
			dir = args[1];
		else
			dir = getenv("HOME");

		if (chdir(dir) == -1)
			perror(dir);
		return;
	}
	if(strcmp(args[0], "pwd") == 0){
		char buf[MAX_INPUT];
		if(getcwd(buf, sizeof buf) != NULL)
			printf("%s\n", buf);
		return;
	}
	if(strcmp(args[0], "exit") == 0){
		exit(0);
	}
}

/* ------------------------------------------------------------------ */
/* execute_command: run an external program and wait for it           */
/* (slides: "Process Creation: fork()", "fork() Return Values",       */
/*  "Program Replacement: exec()", "Waiting for a Child: wait()",      */
/*  "The Shell Loop")                                                  */
/* ------------------------------------------------------------------ */
void execute_command(char **args)
{
	pid_t pid = fork();

	if(pid < 0){
		perror("fork");
		return;
	}

	if (pid == 0){
		execvp(args[0], args);

		perror(args[0]);
		exit(1);
	}

	wait(NULL);

}

/* ------------------------------------------------------------------ */
/* Redirection:  cmd > file   cmd >> file   cmd < file                 */
/* (slides: "File Descriptors", "Redirection with dup2()")            */
/* ------------------------------------------------------------------ */
int has_redirection(char **args)
{
	for(int i = 0; args[i] != NULL; i++){
		if(strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0){
			return 1;
		}
	}
    return 0;
}

void execute_redirection(char **args)
{
	pid_t pid = fork();
	if(pid < 0){
		perror("fork");
		return;
	}

	if (pid == 0){
		int i;

		for (i = 0; args[i] != NULL; i++){
			if(strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0){
				break;
			}
		}
		int fd;
		if (strcmp(args[i], ">") == 0){
			fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		}
		else if (strcmp(args[i], ">>") == 0){
			fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
		}
		else {
			fd = open(args[i + 1], O_RDONLY);
		}
		if (fd < 0){
			perror(args[i + 1]);
			exit(1);
		}
		if (strcmp(args[i], "<") == 0){
			dup2(fd,0);
		}
		else{
			dup2(fd,1);
		}
		close(fd);
		args[i] = NULL;
		execvp(args[0], args);
		perror(args[0]);
		exit(1);
	}
	wait(NULL);
}

/* ------------------------------------------------------------------ */
/* Pipe:  cmd1 | cmd2                                                  */
/* (slides: "Pipes: pipe()", "Pipe Ends and EOF")                     */
/* ------------------------------------------------------------------ */
int has_pipe(char *input)
{
	if(strchr(input, '|') != NULL){
		return 1;
	}
	return 0;
}

void execute_pipe(char *input)
{
	char *pipe_pos = strchr(input, '|');
	*pipe_pos = '\0';
	char *left = input;
	char *right = pipe_pos + 1;
	char *args1[MAX_ARGS];
	char *args2[MAX_ARGS];
	int argc1;
	int argc2;

	parse_input(left, args1, &argc1);
	parse_input(right, args2, &argc2);

	int pipefd[2];
	if (pipe(pipefd) < 0){
		perror("pipe");
		return;
	}
	pid_t pid1 = fork();
	if (pid1 < 0){
		perror("fork");
		return;
	}
	if(pid1 == 0){
		dup2(pipefd[1], 1);
		close(pipefd[0]);
		close(pipefd[1]);
		execvp(args1[0], args1);
		perror(args1[0]);
		exit(1);
	}
	pid_t pid2 = fork();
	if(pid2 < 0){
		perror("fork");
		return;
	}
	if(pid2 == 0){
		dup2(pipefd[0],0);
		close(pipefd[0]);
		close(pipefd[1]);
		execvp(args2[0], args2);
		perror(args2[0]);
		exit(1);
	}
	#ifndef LEAK
		close(pipefd[1]);
	#endif
	close(pipefd[0]);
	wait(NULL);
	wait(NULL);
}
