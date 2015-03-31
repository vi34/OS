all: libhelpers.so libbufio.so
	
libhelpers.so: helpers.c
	gcc -std=c99 -fpic -shared helpers.c -o libhelpers.so

libbufio.so: bufio.c
	gcc -std=c99 -fpic -shared bufio.c -o libbufio.so


clean:
	rm -f libhelpers.so
	rm -f libbufio.so