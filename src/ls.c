#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

char *ls_options[4] = {"-a", "-m", "-am", "-ma"};
int ls_ops[] = {0, 0};


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
				if (source[i] == 'a')
					ls_ops[0] = 1;
				else if (source[i] == 'm')
					ls_ops[1] =1;
				else
					return i;
				i++;
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

int parse_ls(char **argv, int argc)
{
	int i = 1;
	for ( ; i < argc ; ++i)
	{
		int j = 0;
		int found = 0;
		for ( ; j < 4 ; ++j)
		{
			if (strcmp(argv[i], ls_options[j]) == 0)
			{
				ls_ops[j] = 1;
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
//Geeksforgeeks.com
static int myCompare (const void * a, const void * b)
{
	    return strcmp (*(const char **) a, *(const char **) b);
}
	 
void sort(char *arr[], int n)
{
    qsort (arr, n, sizeof (char *), myCompare);
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
	struct dirent *de;
	if (!check_options_driver(argv, argc))
		return 0;
	int index = parse_ls(argv, argc);
	DIR *dr;
	char presentdir[1000];
	if (argv[index] != NULL)
		dr = opendir(argv[index]);
	else dr = opendir(".");
	if (dr == NULL)
	{
		fprintf(stderr, "%s\n", "Directory does not exist");
		return 0;
	}
	char *folders[1000];
	int counter = 0;
	while ((de = readdir(dr)) != NULL)
	{
		folders[counter++] = strdup(de->d_name);
		//printf("%s\n", folders[counter - 1]);
	}
	int i = 0;
	
	
	sort(folders, counter - 1);
	for (i = 0; i < counter; i++)
    {
    	if (ls_ops[0] == 0)
    	{
    		if (folders[i][0] != '.')
    		{
    			if (ls_ops[1] == 1)
    				if (i != counter - 2)
    				{
    					if (isDirectory(folders[i]))
    						printf(BLUE "%s, " RESET, folders[i]);	
    					else
    						printf("%s, ", folders[i]);
    				}	
    				else
    					if (isDirectory(folders[i]))
    						printf(BLUE "%s " RESET, folders[i]);
    					else
    						printf("%s ", folders[i]);
    			else
    				if (isDirectory(folders[i]))
    					printf(BLUE "%s " RESET, folders[i]);	
    				else
						printf("%s ", folders[i]);
    		}
    	}	
		else
		{
			if (ls_ops[1] == 1)
				if (i != counter - 2)
    				if (isDirectory(folders[i]))
						printf(BLUE "%s, " RESET, folders[i]);	
					else
						printf("%s, ", folders[i]);
				else
					if (isDirectory(folders[i]))
						printf(BLUE "%s " RESET, folders[i]);
					else
						printf("%s ", folders[i]);
			else
				if (isDirectory(folders[i]))
					printf(BLUE "%s " RESET, folders[i]);	
				else
					printf("%s ", folders[i]);
		}
	
    }
	
 	printf("\n");
	closedir(dr);    
	
	return 0;
}