all: ush ls rm cat mkdir date help

ush: ush.c	
	gcc -o ush ush.c
cat: cat.c
	gcc -o cat cat.c

mkdir: mkdir.c
	gcc -o mkdir mkdir.c

help: help.c
	gcc -o help help.c

date: date.c
	gcc -o date date.c

ls: ls.c
	gcc -o ls ls.c

rm: rm.c
	gcc -o rm rm.c

clean:
	rm ls cat rm mkdir date help ush
	