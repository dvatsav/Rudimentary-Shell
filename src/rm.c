#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

char *rm_options[4] = {"-v", "-d", "-vd", "-dv"};
int rm_ops[] = {0, 0};

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
				if (source[i] == 'v')
					rm_ops[0] = 1;
				else if (source[i] == 'd')
					rm_ops[1] =1;
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

int parse_rm(char **argv, int argc)
{
	int i = 1;
	for ( ; i < argc ; ++i)
	{
		int j = 0;
		int found = 0;
		for ( ; j < 4 ; ++j)
		{
			if (strcmp(argv[i], rm_options[j]) == 0)
			{
				rm_ops[j] = 1;
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

int dir_entries(char *foldername)
{
	struct dirent *de;
	DIR *dr = opendir(foldername);
	if (dr == NULL)
	{
		return -1;
	}
	int counter = -2;
	while ((de = readdir(dr)) != NULL)
		counter++;
	return counter;
}
//geeksforgeeks.com
int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int main(int argc, char **argv)
{

	if (!check_options_driver(argv, argc))
		return 0;
	int index = parse_rm(argv, argc);
	int i = index;
	for ( ; i < argc ; ++i)
	{
		if (rm_ops[1] == 1)
		{
			int result = dir_entries(argv[i]);
			if (is_regular_file(argv[i]))
			{
				remove(argv[i]);
				if (rm_ops[0] == 1)
					printf("%s%s%s\n", "removed file: '", argv[i], "'");	
				continue;
			}

			if (result == -1)
			{
				fprintf(stderr, "%s%s\n", "rm: Invalid directory name: ", argv[i]);
				continue;
			}
			else if (result > 0)
			{
				//chdir(argv[index]);
				fprintf(stderr, "%s%s\n", "rm: Directory not empty: ", argv[i]);
				continue;
			}
			else
			{
				if (rm_ops[0] == 1)
					printf("%s%s%s\n", "removed directory: '", argv[i], "'");
				rmdir(argv[i]);
			}
		}
		else if ( !is_regular_file(argv[i]) && rm_ops[1] == 0)
		{
			fprintf(stderr, "%s%s\n", "Error: File does not exist / use -d if you want to delete an empty folder: ", argv[i]);
		}
		else if (is_regular_file(argv[i]))
		{
			remove(argv[i]);
			if (rm_ops[0] == 1)
				printf("%s%s%s\n", "removed file: '", argv[i], "'");	
		}	
	}
	
	return 0;
}