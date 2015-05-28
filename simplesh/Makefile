NAME = simplesh

all: ../lib/libbufio.so
	gcc -std=c99 -Wall -L../lib/ -lbufio -lhelpers -Wl,-rpath,../lib/ $(NAME).c -o $(NAME)

../lib/libbufio.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 

clean:
	rm -f $(NAME)