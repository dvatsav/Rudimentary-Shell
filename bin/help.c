#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		fprintf(stderr, "%s\n", "help does not take any arguments");
		return 0;
	}
	else
	{
		printf("%s\n", "echo: options - [-n, -e] -- print to a stream\n\t-n: Dont start on new line\n\t-e: take into account escape sequences such as \\n\nhistory: options - [-c, <any number>] -- View Command History\n\t-c: clear history\n\tnumber: print last <number> of commands from history\ncd: paths = [-, ~, ., .., <any valid filepath>] -- Change Directory\n\t-: last working directory\n\t~: Home directory\n\t.: current directory\n\t..: previous directory in hierarchy\npwd -- Present Working Directory\nexit -- Exit Ultimate Shell\nls: options - [-a, -m] -- List\n\t-a: view all files\n\t-m: comma separate the output\ncat: options - [-n, -E] -- Concatenate\n\t-n: number the lines\n\t-E: End line with $\ndate: options - [-u, -R] -- Date Function\n\t-u: UTC time\n\t-R: output date and time in RFC 2822 format.  Example: Mon, 07 Aug 2006 12:34:56 -0600\nmkdir: options - [-v, -m] -- Make Directory\n\t-v: Verbose output\n\t-m: specify file mode\nrm: options - [-v, -d] -- Remove\n\t-v: Verbose\n\t-d: remove empty directory");
	}
	return 0;
}