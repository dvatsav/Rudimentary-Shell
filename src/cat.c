#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

char *cat_options[4] = {"-n", "-E", "-nE", "-En"};
int cat_ops[] = {0, 0};

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
					cat_ops[0] = 1;
				else if (source[i] == 'E')
					cat_ops[1] =1;
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

int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}


int parse_cat(char **argv, int argc)
{
	int i = 1;
	for ( ; i < argc ; ++i)
	{
		int j = 0;
		int found = 0;
		for ( ; j < 4 ; ++j)
		{
			if (strcmp(argv[i], cat_options[j]) == 0)
			{
				cat_ops[j] = 1;
				found = 1;
			}	
		}
		if (!found)
		{
			return i;
		}
	}
	return argc;
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

int main(int argc, char **argv)
{
	if (!check_options_driver(argv, argc))
		return 0;
	if (argc == 1)
	{
		while(1)
		{
			char *command = ush_readline();
			printf("%s\n", command);

		}
		return 0;
	}
	if (!strcmp(argv[1], "*"))
	{
		struct dirent *de; 	
		DIR *dr;
		dr = opendir(".");
		while ((de = readdir(dr)) != NULL)
		{
			if(de->d_name[0] != '.')
			{
				if (isDirectory(de->d_name))
					printf("%s%s%s\n", "cat: ", de->d_name, " is a directory");
				else
				{
					FILE *fp = fopen(de->d_name, "r");
					char c = fgetc(fp);
					while (c != EOF)
					{
						printf ("%c", c);
						c = fgetc(fp);
					}
				}
			}
		}
		return 0;
	}
	int index = parse_cat(argv, argc);
	while (index < argc)
	{
		if (!strcmp(argv[index], "-n") || !strcmp(argv[index], "-E") || !strcmp(argv[index], "-nE") || !strcmp(argv[index], "-En"))
		{
			index++;
			continue;
		}
		if (isDirectory(argv[index]))
		{
			fprintf(stderr, "%s%s%s\n", "cat: ", argv[index], " is a directory");
		}
		FILE *fp = fopen(argv[index], "r");
		if (fp == NULL)
		{
			fprintf(stderr, "%s\n", "file does not exist");
			return 0;
		}
		if (cat_ops[0] == 0 && cat_ops[1] == 0)
		{
			char c = fgetc(fp);
			while (c != EOF)
			{
				printf ("%c", c);
				c = fgetc(fp);
			}	
		}
		else if (cat_ops[0] == 1)
		{
			char line[1024];
			
			
			int counter = 1;
			while (fgets(line, 1024, fp) != NULL)
			{
				char *pos;
				if ((pos = strchr(line, '\n')) != NULL)
	   				*pos = '\0';
				if (cat_ops[1] == 1)
				{
					printf("%d %s%s", counter++, line, "$");
				}
				else
				{
					printf("%d %s", counter++, line);
				}
				printf("\n");
			}
		}
		else
		{
			char line[1024];
			while (fgets(line, 1024, fp) != NULL)
			{
				char *pos;
				if ((pos = strchr(line, '\n')) != NULL)
	   				*pos = '\0';
				printf("%s%s\n", line, "$");
			}
		}
		fclose(fp);	
		index++;
	}
		
	
	return 0;
}