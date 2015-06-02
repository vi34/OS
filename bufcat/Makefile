NAME = bufcat

all: $(NAME)
	
$(NAME): $(NAME).c ../lib/libbufio.so
	gcc -std=c99 -o $(NAME) $(NAME).c -I../lib -Wl,-rpath,../lib/ -L../lib/ -lbufio  

../lib/libbufio.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 

clean:
	rm -f $(NAME)
