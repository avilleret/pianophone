all:
	g++ -o pianophone -l rt pianophone.cpp -l bcm2835 -l lo
	gcc -o lcd -l rt lcd.c

clean:
	rm pianophone
	rm lcd
