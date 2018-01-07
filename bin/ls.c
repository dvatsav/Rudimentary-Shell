#include <stdio.h>
#include <dirent.h>
#include <string.h>

char *ls_options[3] = {"-a", "-m", "-1"};
int ls_ops[] = {0, 0, 0};


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
				if (source[i] == 'a' || source[i] == 'm' || source[i] == '1')
				{
					++i;
					if (source[i] == 'a')
						ls_ops[0] = 1;
					else if (source[i] == 'm')
						ls_ops[1] =1;
					else if (source[i] == '1')
						ls_ops[2] =1;
					continue;
				}
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

int parse_ls(char **argv, int argc)
{
	int i = 1;
	for ( ; i < argc ; ++i)
	{
		int j = 0;
		int found = 0;
		for ( ; j < 3 ; ++j)
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

int main(int argc, char **argv)
{
	struct dirent *de;
	if (!check_options_driver(argv, argc))
		return 0;
	int index = parse_ls(argv, argc);
	DIR *dr;
	
	if (argv[index] != NULL)
		dr = opendir(argv[index]);
	else dr = opendir(".");
	if (dr == NULL)
	{
		fprintf(stderr, "%s\n", "Directory does not exist");
		return 0;
	}
	while ((de = readdir(dr)) != NULL)
	{
		if (ls_ops[0] == 0)
		{
			if(de->d_name[0] != '.')
			{
				if (ls_ops[1] == 1)
					printf("%s, ", de->d_name);
				if (ls_ops[2] == 1)
					printf("%s\n", de->d_name);	
				if(ls_ops[1] == 0 && ls_ops[2] == 0)
					printf("%s ", de->d_name);
			}
		}
		else
		{
			if (ls_ops[1] == 1)
					printf("%s, ", de->d_name);
			if (ls_ops[2] == 1)
				printf("%s\n", de->d_name);	
			if(ls_ops[1] == 0 && ls_ops[2] == 0)
				printf("%s ", de->d_name);
		}
			
	}
 	printf("\n");

	closedir(dr);    
	return 0;
}