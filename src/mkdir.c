#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *mkdir_options[3] = {"-v", "-m", "-vm"};
int mkdir_ops[] = {0, 0};
int mindex;

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
					mkdir_ops[0] = 1;
				else if (source[i] == 'm')
					mkdir_ops[1] =1;
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

int parse_mkdir(char **argv, int argc)
{
	int i = 1;
	while (i < argc)
	{
		int j = 0;
		int found = 0;
		for ( ; j < 3 ; ++j)
		{
			
			if (strcmp(argv[i], mkdir_options[j]) == 0)
			{
				mkdir_ops[j] = 1;
				if (j == 1)
				{
					mindex = i;
					found = 1;
					++i;
				}
				else found = 1;
			}	
		}
		if (!found)
		{
			return i;
		}
		++i;
	}

	return argc;
}

int check_valid_mode(char *mode)
{
	int i = 0;
	if (strlen(mode) != 3 && strlen(mode) != 4)
		return 0;
	//printf("%d\n", mode[0] - '0');
	if (strlen(mode) == 3)
	{
		if (mode[0] - '0' < 0 || mode[0] - '0' > 7)
			return 0;
		if (mode[1] - '0' < 0 || mode[1] - '0' > 7)
			return 0;
		if (mode[2] - '0' < 0 || mode[2] - '0' > 7)
			return 0;
	}
	if (strlen(mode) == 4)
	{
		if (mode[0] - '0' < 0 || mode[0] - '0' > 1)
			return 0;
		if (mode[3] - '0' < 0 || mode[3] - '0' > 7)
			return 0;
		if (mode[1] - '0' < 0 || mode[1] - '0' > 7)
			return 0;
		if (mode[2] - '0' < 0 || mode[2] - '0' > 7)
			return 0;
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

int main(int argc, char **argv)
{
	if (!check_options_driver(argv, argc))
		return 0;
	struct stat st = {0};
	int index = parse_mkdir(argv, argc);
	int i = index;
	for ( ; i < argc ; ++i)
	{
		if (stat(argv[i], &st) == -1)
		{
			if (mkdir_ops[1] == 1)
				{
					if (check_valid_mode(argv[mindex + 1]))
					{
						umask(0);
						printf("%d\n", str_to_int(argv[mindex + 1]));
						char *ptr;
						long ret = strtoul(argv[mindex + 1], &ptr, 8); 
						mkdir(argv[i], ret);
						umask(0022);
					}
					else
					{
						fprintf(stderr, "%s%s\n", "mkdir: invalid mode ", argv[mindex + 1]);
						return 0;
					}
				}
			else 
				mkdir(argv[i], 0700);
			if (mkdir_ops[0] == 1)
			{
				printf("%s%s%s\n", "mkdir: created directory '", argv[i], "'");
			}
			
		}
		else
		{
			fprintf(stderr, "%s%s%s\n", "mkdir: ", argv[i] ," directory already exists");
		}
	}
	return 0;
}