#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <ctype.h>
#include <dirent.h>
#include <signal.h>

#define delimiters " \"\n\t\r"
#define MAX_BUFFER_SIZE 1024
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

char prev_directory[1024];
char *echo_options[4] = {"-n", "-e", "-ne", "-en"};
int echo_ops[] = {0, 0};

char global_path[1000];

char *allarguments[12] = {"cd", "echo", "pwd", "exit", "history", "ls", "date", "rm", "mkdir", "help", "cat", "clear"};

struct ar
{
	char **argument;
};
int inputhandler = 0;
void sig_handler(int signo)
{
     printf("%s", "\n");
     inputhandler = 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	Exit the program
*/
void shell_exit()
{
	exit(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	change directory (cd) operations
*/

void shell_cd(char **arguments)
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	if (arguments[1] == NULL)
	{
		chdir(getenv("HOME"));
	}
	else if (arguments[1][0] == '~')
	{
		char *dest = malloc(1024 * sizeof(char));
		strcpy(dest, getenv("HOME"));
		char *path = arguments[1];
		path++;
		strcat(dest, path);
		if (chdir(dest) != 0) {
			perror(dest);
		}
	}
	else if (arguments[1][0] == '-') 
	{
		if (chdir(prev_directory) != 0)
			perror(prev_directory);
	}
	else if (chdir(arguments[1]) != 0)
	{
		perror(arguments[1]);
	}	

	strcpy(prev_directory, cwd);
	
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	history storage
*/

int check_str_num(char num[1024])
{
	int i = 0;
	for ( ; i < strlen(num) ; ++i)
	{
		if ((int)num[i] < 48 || (int)num[i] > 57)
			return -1;
	}
	return 1;
}

int str_to_int(char num[1024])
{
	int len = strlen(num);
	int dec = 0, i = 0;
	for(i = 0 ; i < len ; ++i)
	{
		dec = dec * 10 + (num[i] - '0');
	}

	return dec;

}

void shell_history(int write, char *command, char *historyhome, char **arguments)
{

	int index = 0;
	if (write)
	{
		FILE *his = fopen(historyhome, "a");
		fprintf(his, "%s\n", command);
		fclose(his);
	}
	else
	{
		if (arguments[1] != NULL && strcmp(arguments[1], "-c") == 0)
		{
			FILE *his = fopen(historyhome, "w");
			fclose(his);
			return;
		}
		else if (arguments[1] != NULL && check_str_num(arguments[1]) == 1)
		{
			FILE *his = fopen(historyhome, "r");
			int linecount = 0;
			char line[1024];
			while (fgets(line, 1024, his) != NULL)
			{
				linecount = linecount + 1;
			}
			fclose(his);
			his = fopen(historyhome, "r");
			int toprint = str_to_int(arguments[1]);
			char line2[1024];
			int counter = 1;
			

			int flag = 0;
			if (toprint > linecount)
				flag = 1;
			while (fgets(line2, 1024, his) != NULL)
			{
				if (counter == linecount - toprint + 1)
					flag = 1;
				if (flag)
					printf("%d %s",counter, line2);
				counter++;
			}
			fclose(his);
			return;
		}
		else if (arguments[1] != NULL && check_str_num(arguments[1]) == -1)
		{
			fprintf(stderr, "%s\n", "bash: invalid arguments");
			return;
		}

		FILE *his = fopen(historyhome, "r");
		char line[1024];
		int counter = 1;
		while (fgets(line, sizeof(line), his) != NULL)
		{
			printf("%d %s",counter++, line);
		}
		fclose(his);
	}	
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	show present working directory
*/
void shell_pwd()
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("%s\n", cwd);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	Readline buffer function
*/

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the null-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
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

char *concat_paths(char *buffer)
{
	char *paths = concat(global_path, "/");
	char *buf = concat(paths, buffer);
	return buf;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	parsing functions
*/

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

int ush_parse_echo_options(char **arguments, int size)
{
	int i = 1;
	for ( ; i < size ; ++i)
	{
		int j = 0, flag = 0;
		for ( ; j < 4 ; ++j)
		{
			if (strcmp(echo_options[j], arguments[i]) == 0)
			{
				echo_ops[j] = 1;
				flag = 1;
			}
		}
		if (!flag)
			return i;
	}
	return size;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	Echo command
*/

int check(char *source)
{
	int i = 0;
	while (i < strlen(source))
	{
		if (source[i] == '-')
		{
			++i;
			while (i < strlen(source))
			{

				if (source[i] == 'n')
					echo_ops[0] = 1;
				else if (source[i] == 'e')
					echo_ops[1] =1;
				else
				return i;
				++i;
			}
		}
		++i;
	}
	return -1;
}

int check_options_driver(char **argv, int argc)
{
	int i = 0;
	for ( ; i < argc ; ++i)
	{
		int p;
		if ( (p = check(argv[i])) != -1)
		{
			fprintf(stderr, "%s%s\n", "Invalid argument: ", argv[i]);
			return 0;
		}
	}
	return 1;
}

void exec_echo(char *source, char *final, int *c)
{
	if (echo_ops[1] == 0)
	{
		strcpy(final, source);
		return;
	}
	int i = 0, j = 0, esc = 0;
	while (i < strlen(source))
	{
		if (source[i] != '\\')
		{
			final[j++] = source[i];
		}
		else
		{
			esc = 1;
			i++;	
		} 
		if (esc == 1)
		{
			if (source[i] == 'b')
				final[j++] = '\b';
			else if (source[i] == 'a')
				final[j++] = '\a';
			else if (source[i] == 'n')
				final[j++] = '\n';
			else if (source[i] == 't')
				final[j++] = '\t';
			else if (source[i] == 'v')
				final[j++] = '\v';
			else if (source[i] == 'c')
			{
				*c = 1;
				break;
			}
			esc = 0;
		}
		++i;
	}
}

void ush_echo(char **arguments, int index, int total_args)
{
	int i = index;
	int echo_ops_c = 0;
	for ( ; i < total_args ; ++i)
	{
		char *final = malloc(1024 * sizeof(char));
		exec_echo(arguments[i], final, &echo_ops_c);
		if (strcmp(arguments[1], "*") == 0)
		{
			struct dirent *de; 	
			DIR *dr;
			dr = opendir(".");
			while ((de = readdir(dr)) != NULL)
			{
				if(de->d_name[0] != '.')
				{
					printf("%s ", de->d_name);
				}
			}
		}
		else
			printf("%s ", final);
		strcpy(final, "");
		if (echo_ops_c == 1)
			break;
	}
	if (echo_ops[0] == 0 && echo_ops_c == 0)
	{
		printf("\n");
		echo_ops_c = 0;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	Execute commands by creating child process
*/

void execute_command(char **arguments)
{
	pid_t pid = fork();
	
	if (pid == 0)
	{
		execv(arguments[0], arguments);
		//execvp(arguments[0], arguments);
		//perror(arguments[0]);
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

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	pipelining
*/

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

void shell_clear()
{
	system("clear");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

/*
	counts the quotes in the input given by the user
*/

int count_quotes(char *command)
{
	int i, count;
	for (i=0, count=0; command[i]; i++)
  		count += (command[i] == '"');
  	return count;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

void substring(char *s, char *sub, int p, int l)
{
	int c = 0;

	while (c < l) 
	{
  		sub[c] = s[p + c - 1];
  		c++;
	}
	sub[c] = '\0';
}


int main(int argc, char **argv)
{
	if (signal(SIGINT, &sig_handler) == SIG_ERR)
  		printf("\ncan't catch SIGINT\n");
	char historyhome[1024];
	getcwd(global_path, sizeof(global_path));
	strcpy(prev_directory, "/");
	strcpy(historyhome, getenv("HOME"));
	strcat(historyhome, "/.ush_history");
	while (1)
	{
		
		if (inputhandler == 1)
		{
			inputhandler = 0;
			continue;
		}
		char *command;
		char *commandcopy = malloc(1024 * sizeof(char));
		strcpy(commandcopy, "");
		char cwd[1024];
		char *user = strcat(getenv("USER"), ":");
        char prompt[1024] = "";
        
		getcwd(cwd, sizeof(cwd));
        strcpy(prompt, strcat(cwd, ">"));
		printf(CYAN "%s" RESET, prompt);
		
		command = ush_readline();
		commandcopy = strdup(command);
		command = concat_paths(command);
		//Command is returned with the path to the exexcutable appeneded to it
		//printf("%d\n", strlen(cwd));
		//substring(command, commandcopy, strlen(cwd) + 5, strlen(command) - strlen(cwd) + 4);
		if (strlen(commandcopy) == 0)
		{
			continue;
		}
		//substring without path to executables is extracted here and used for pipeline
		
		if (count_quotes(command) % 2 != 0)
		{
			//printf("%s\n", command);
			char *extra;
			do
			{
				printf("%s", ">");
				extra = ush_readline();
				strcat(command, extra);
				//printf("%s\n", command);
			}while(count_quotes(extra) % 2 == 0);

			strcat(command, "\"");
		}
		
		char *temp[1024];
		shell_history(1, commandcopy, historyhome, temp);
		char **arguments = ush_parser(command); //Used for normal commands
		char **pipearg = ush_parser(commandcopy); //used for pipeline commands
		
		char **count = arguments;
		int redcount = 0, total_args = 0;
		
		while (*count != NULL)
		{
			total_args++;
			if (strcmp(*count, "|") == 0 || strcmp(*count, ">>") == 0 || strcmp(*count, ">") == 0 || strcmp(*count, "<") == 0)
			{
				redcount++;
			}
			count++;
		}

		int g = 0, flag = 0;
		for ( ; g < 12 ; ++g)
		{
			if (!strcmp(pipearg[0], allarguments[g]))
			{
				flag = 1;
				break;
			}
		}	
		if (!flag)
		{
			fprintf(stderr, "%s\n", "That command does not exist. Try 'help' to view all commands");
			continue;
		}
		
		
		if (strcmp(pipearg[0], "exit") == 0)
			shell_exit();
		else if (strcmp(pipearg[0], "cd") == 0)
			shell_cd(arguments);
		else if (strcmp(pipearg[0], "clear") == 0)
			shell_clear();
		else if (strcmp(pipearg[0], "history") == 0)
			shell_history(0, NULL, historyhome, arguments);
		else if (strcmp(pipearg[0], "pwd") == 0)
			shell_pwd();
		else if (strcmp(pipearg[0], "echo") == 0)
		{	
			if (!check_options_driver(pipearg, total_args))
			{
				continue;
			}
			int index = ush_parse_echo_options(arguments, total_args);
			ush_echo(arguments, index, total_args);
		}
		else if (redcount > 0)
		{
			redcount++;
			int counter = 0, end = 0, start = 0;
			struct ar argument_array[redcount];
			char **args = pipearg;
			
			while (*args != NULL)
			{
				if (strcmp(*args, "|") == 0 || strcmp(*args, ">>") == 0 || strcmp(*args, ">") == 0 || strcmp(*args, "<") == 0)
				{
					char **arglist = malloc(end * sizeof(char*));
					memcpy(arglist, &pipearg[start], (end - start) * sizeof(char*));
					arglist[end - start + 1] = NULL;
					argument_array[counter++].argument = arglist;
					start = end + 1;
				}
				end++;
				args++;
			}
			char **arglist = malloc(end * sizeof(char*));
			memcpy(arglist, &pipearg[start], (end - start) * sizeof(char*));
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

//---------------------------------------------------------------------------------------------------------------------------------------------------