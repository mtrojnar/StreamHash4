# Makefile by Michal Trojnara 2016

all:
	$(MAKE) -C src
	$(MAKE) -C tests

clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean
