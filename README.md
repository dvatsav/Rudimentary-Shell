# Rudimentary-Shell

Operating Systems Assignment
Working:
The file ush.c contains the source code of the shell. The internal commands (mentioned below) are implemented within this source code.

The user input is parsed, and then checked as to whether it is an internal command. If so, it is executed as per the source code in ush.c --
	- echo: arguments parsed and appropriately printed to stdout (does not support ``)
	- history: reads hidden file ush_history that is present in the home directory.
	- cd: Memoization of previous directories visited along with chdir() command in C
	- pwd: getcwd() command of C
	- exit: exit()
For external commands, a child process is created and execv() is used to execute the appropriate binary, after which the child process is terminated and control is returned to the parent process.
	- ls: makes use of dirent.h
	- cat: reads a file and prints to stdout
	- date: uses time.h
	- mkdir: uses sys/stat.h to determine existence of a directory
	- rm: uses rmdir(), remove() and dirent.h. 

Extra:
	pipelining implemented as well. However for the pipelining commands, I have used the default linux binaries and not the ones I have written. 
	Assumption:
		syntax is like: ls | sort, where there is a space between pipeline and command

Note:
	system("clear") used for convenience

Internal Commands Implemented:

echo: options - [-n, -e]
history: options - [-c, <any number>]
cd: paths = [-, ~, ., .., <any valid filepath>]
pwd
exit

Error checks:
	- history and echo checked for invalid arguments
	- cd checked for invalid file path

Assumption for cd:
	- No gaps in file names
Assumption for echo:
	- echo "
	fsdfs
	sdfsd" not allowed
	- echo "Asda
	adsasd
	asda" will work
External Commands Implemented:

ls: options - [-a, -m]
cat: options - [-n, -E]
date: options - [-u, -R]
mkdir: options - [-v, -m]
rm: options - [-v, -d]

Error checks for external commands: 
	- invalid options
	- invalid file/folder/directory
