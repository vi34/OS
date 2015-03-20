all: ../lib/libhelpers.so
	gcc -g -Wall -L../lib/ -lhelpers -Wl,-rpath,../lib/ cat.c -o cat

../lib/libhelpers.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 

clean:
	rm -f cat
	rm -rf cat.dSYM