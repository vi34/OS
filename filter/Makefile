NAME = filter

all: $(NAME)
	
$(NAME): $(NAME).c ../lib/libhelpers.so
	gcc -std=c99 -o $(NAME) $(NAME).c -I../lib -Wl,-rpath,../lib/ -L../lib/ -lhelpers  

../lib/libhelpers.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 

clean:
	rm -f $(NAME)
