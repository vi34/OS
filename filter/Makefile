NAME = filter

all: ../lib/libhelpers.so $(NAME)
	
$(NAME): $(NAME).c
	gcc -std=c99 -L../lib/ -lhelpers -Wl,-rpath,../lib/ $(NAME).c -o $(NAME)

../lib/libhelpers.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 

clean:
	rm -f $(NAME)