NAME = simplesh

all: ../lib/libhelpers.so simplesh
	
simplesh: simplesh.c
	gcc -std=c99 -L../lib/ -lbufio -lhelpers -Wl,-rpath,../lib/ $(NAME).c -o $(NAME)

../lib/libhelpers.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 

clean:
	rm -f $(NAME)