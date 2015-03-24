all: libhelpers.so
	
libhelpers.so: helpers.c
	gcc -std=c99 -fpic -shared helpers.c -o libhelpers.so

clean:
	rm -f libhelpers.so