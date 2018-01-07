#include <time.h>
#include <stdio.h>
#include <string.h>

char *date_options[4] = {"-R", "-u", "-uR", "-Ru"};
int date_ops[] = {0, 0};

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
				if (source[i] == 'R')
					date_ops[0] = 1;
				else if (source[i] == 'u')
					date_ops[1] =1;
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


int parse_date(char **argv, int argc)
{
	int i = 1;
	for ( ; i < argc ; ++i)
	{
		int j = 0;
		int found = 0;
		for ( ; j < 4 ; ++j)
		{
			if (strcmp(argv[i], date_options[j]) == 0)
			{
				date_ops[j] = 1;
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
	if (!check_options_driver(argv, argc))
		return 0;
	time_t t = time(NULL);
	int index = parse_date(argv, argc);
	struct tm *tm;
	if (date_ops[0] == 0)
	{
		if (date_ops[1] == 1)
			tm = gmtime(&t);
		else
			tm = localtime(&t);
		char s[64];
		strftime(s, sizeof(s), "%c", tm);
	    printf("%s\n", s);
	}
	else
	{
		if (date_ops[1] == 1)
			tm = gmtime(&t);
		else
			tm = localtime(&t);
		char s[64];
		strftime(s, sizeof(s), "%a, %d %h %Y %T %z", tm);
		printf("%s\n", s);
	}
	return 0;
}
