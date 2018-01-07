#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

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
	if (rm_ops[1] == 1)
	{
		int result = dir_entries(argv[index]);
		if (result == -1)
		{
			fprintf(stderr, "%s\n", "mkdir: Invalid directory name");
			return 0;
		}
		else if (result > 0)
		{
			chdir(argv[index]);
			fprintf(stderr, "%s\n", "mkdir: Directory not empty");
			return 0;
		}
		else
		{
			if (rm_ops[0] == 1)
				printf("%s%s%s\n", "removed directory: '", argv[index], "'");
			rmdir(argv[index]);
		}
	}
	else if ( !is_regular_file(argv[index]) && rm_ops[1] == 0)
	{
		fprintf(stderr, "%s\n", "Error: File does not exist / use -d if you want to delete an empty folder");
	}
	else if (is_regular_file(argv[index]))
	{
		remove(argv[index]);
		if (rm_ops[0] == 1)
			printf("%s%s%s\n", "removed file: '", argv[index], "'");	
	}
	return 0;
}