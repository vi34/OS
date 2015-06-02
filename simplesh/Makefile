NAME = simplesh

all: $(NAME) 

$(NAME): $(NAME).c ../lib/libhelpers.so ../lib/libbufio.so
	gcc -std=c99 -o $(NAME) $(NAME).c -I../lib -Wl,-rpath,../lib/ -L../lib/ -lhelpers -lbufio

../lib/libhelpers.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 

../lib/libbufio.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 


clean:
	rm -f $(NAME)