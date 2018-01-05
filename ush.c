#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>

#define delimiters " \n\t"
#define MAX_BUFFER_SIZE 1024

struct ar
{
	char **argument;
};

struct pipelines
{
	int fd[2];
};

char prev_directory[1024];
void shell_exit()
{
	exit(0);
}

void shell_cd(char **arguments)
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	if (arguments[1] == NULL)
	{
		char *home = getenv("HOME");
		chdir(home);
	}
	else if (arguments[1][0] == '~')
	{
		char *dest = getenv("HOME");
		char *path = arguments[1];
		path++;
		strcat(dest, path);
		if (chdir(dest) != 0) {
			perror("Error");
		}
	}
	else if (arguments[1][0] == '-') 
	{
		if (chdir(prev_directory) != 0)
			perror("error: ");
	}
	else if (chdir(arguments[1]) != 0)
	{
		perror("error:");
	}	

	strcpy(prev_directory, cwd);
	
}

char *ush_readline()
{
	ssize_t buffer_size = 0;
	char *buffer = NULL;
	getline(&buffer, &buffer_size, stdin);
	char *pos;
	if ((pos = strchr(buffer, '\n')) != NULL)
   		*pos = '\0';
	return buffer;
	
}

char **ush_parser(char *command)
{
	int buffer_size = MAX_BUFFER_SIZE;
	int index = 0;
	char **arguments = malloc(sizeof(char*) * buffer_size);
	char *argument;

	argument = strtok(command, delimiters);
	while (argument != NULL)
	{
		arguments[index++] = argument;
		if (index >= buffer_size)
		{
			buffer_size += MAX_BUFFER_SIZE;
			arguments = realloc(arguments, sizeof(char*) * buffer_size);
		}
		argument = strtok(NULL, delimiters);
	}
	arguments[index] = NULL;
	return arguments;
}

void execute_command(char **arguments)
{
	pid_t pid = fork();
	if (pid == 0)
	{
		execvp(arguments[0], arguments);
		perror("Error: ");
		exit(0);
	}
	else if (pid < 0)
	{
		perror("Error creating fork: ");
	}
	else
	{
		waitpid(pid, 0, 0);
	}
}

int execute_pipeline(char **args, int input, int output)
{
	pid_t pid;
	char buf;
	
	int fd[2];
	if (pipe(fd) == -1)
	{
		fprintf(stderr, "%s\n", "Error Creating pipe");
	}	
	pid = fork();
	if (!pid)
	{
		if (input == 0)
		{
			close(fd[0]);
			dup2(fd[1], 1);
			close(fd[1]);	
			execvp(args[0], args);
			perror(args[0]);
			exit(0);
		}
		else if (output == 1)
		{
			close(fd[0]);
			close(fd[1]);
			dup2(input, 0);
			execvp(args[0], args);
			perror(args[0]);
			close(input);
			exit(0);
		}
		else
		{
			dup2(input, 0);
			close(fd[0]);
			dup2(fd[1], 1);
			close(fd[1]);
			execvp(args[0], args);
			perror(args[0]);
			close(input);
			exit(0);
		}
	} else if (pid < 0) 
	{
		fprintf(stderr, "%s\n", "Error forking");
	} else 
	{
		waitpid(pid, 0, 0);
	}

	close(fd[1]);
	if (output == 1)
	{
		close(fd[0]);
		close(input);
	}
	if (input)
	{
		close(input);
	}
	return fd[0];
}

int main(int argc, char **argv)
{
	strcpy(prev_directory, "/");
	while (1)
	{
		char *command;
		char cwd[1024];
		getcwd(cwd, sizeof(cwd));
        char *user = strcat(getenv("USER"), ":");
        char prompt[1024] = "";
        strcpy(prompt, strcat(cwd, ">"));
		printf("%s", prompt);
		command = ush_readline();
		char **arguments = ush_parser(command);
		char **count = arguments;
		int redcount = 0;
		
		while (*count != NULL)
		{
			if (strcmp(*count, "|") == 0 || strcmp(*count, ">>") == 0 || strcmp(*count, ">") == 0 || strcmp(*count, "<") == 0)
			{
				redcount++;
			}
			count++;
		}
		char *pipes_redirs[redcount];
		if (strcmp(arguments[0], "exit") == 0)
			shell_exit();
		else if (strcmp(arguments[0], "cd") == 0)
			shell_cd(arguments);
		else if (redcount > 0)
		{
			redcount++;
			int counter = 0, end = 0, start = 0;
			struct ar argument_array[redcount];
			char **args = arguments;
			
			while (*args != NULL)
			{
				if (strcmp(*args, "|") == 0 || strcmp(*args, ">>") == 0 || strcmp(*args, ">") == 0 || strcmp(*args, "<") == 0)
				{
					pipes_redirs[counter] = (char*)args;
					char **arglist = malloc(end * sizeof(char*));
					memcpy(arglist, &arguments[start], (end - start) * sizeof(char*));
					arglist[end - start + 1] = NULL;
					argument_array[counter++].argument = arglist;
					start = end + 1;
				}
				end++;
				args++;
			}
			char **arglist = malloc(end * sizeof(char*));
			memcpy(arglist, &arguments[start], (end - start) * sizeof(char*));
			argument_array[counter++].argument = arglist;

			int i = 0, input = 0;
			while(i < redcount)
			{
				
				if (i != redcount - 1)
					input = execute_pipeline(argument_array[i].argument, input, 0);
				else
					input = execute_pipeline(argument_array[i].argument, input, 1);
				++i;
				
			}
		}
		else
			execute_command(arguments);
	}
	return 0;
}