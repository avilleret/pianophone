all:
	gcc -o pianophone -l rt pianophone.c -l bcm2835 -l lo

clean:
	rm pianophone
