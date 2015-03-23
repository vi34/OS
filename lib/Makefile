all: libhelpers.so
	
libhelpers.so:
	gcc -std=c99 -fpic -shared helpers.c -o libhelpers.so
clean:
	rm -f libhelpers.so