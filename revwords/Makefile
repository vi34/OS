all: ../lib/libhelpers.so
	gcc -g -Wall -L../lib/ -lhelpers  -Wl,-rpath,../lib/ revwords.c -o revwords

../lib/libhelpers.so:
	$(MAKE) -C $(dir $@) $(notdir $@) 

clean:
	rm -f revwords
	rm -rf revwords.dSYM