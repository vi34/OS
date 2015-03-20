all: libhelpers.so
	
libhelpers.so:
	gcc -g -Wall -fpic -shared helpers.c -o libhelpers.so
clean:
	rm -f libhelpers.so
	rm -rf libhelpers.so.dSYM