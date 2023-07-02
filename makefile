# make makefile
#
# Tools used:
#  Compile::Resource Compiler
#  Compile::GNU C
#  Make: make
all : toolbar.exe

toolbar.exe : toolbar.obj toolbar.def toolbar.res
	gcc -Zomf -g toolbar.obj toolbar.def toolbar.res -o toolbar.exe
	wrc -q toolbar.res

toolbar.obj : toolbar.c toolbar.h
	gcc -Wall -Zomf -c -x c -g toolbar.c -o toolbar.obj

toolbar.res : toolbar.rc
	wrc -q -r -bt=os2 toolbar.rc

clean :
	rm -rf *exe *res *obj *lib
