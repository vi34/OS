all: ../lib/libhelpers.so
	gcc -g -Wall -L../lib/ -lhelpers -Wl,-rpath,../lib/ lenwords.c -o lenwords

../lib/libhelpers.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 

clean:
	rm -f lenwords
	rm -rf lenwords.dSYM