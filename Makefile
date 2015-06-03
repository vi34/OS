all: 
	$(MAKE) -C lib
	$(MAKE) -C cat
	$(MAKE) -C revwords
	$(MAKE) -C filter
	$(MAKE) -C bufcat
	$(MAKE) -C buffilter
	$(MAKE) -C simplesh
	$(MAKE) -C filesender
	$(MAKE) -C bipiper
clean:
	$(MAKE) -C lib clean
	$(MAKE) -C cat clean
	$(MAKE) -C revwords clean
	$(MAKE) -C filter clean
	$(MAKE) -C bufcat clean
	$(MAKE) -C buffilter clean
	$(MAKE) -C simplesh clean
	$(MAKE) -C filesender clean
	$(MAKE) -C bipiper clean
