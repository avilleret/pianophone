all:
	g++ -o pianophone -l rt pianophone.cpp -l bcm2835 -l lo

clean:
	rm pianophone
	rm lcd
