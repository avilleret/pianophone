all: pianophone lcd

pianophone:
	g++ -o pianophone -l rt pianophone.cpp -l bcm2835 -l lo
	
lcd:
	g++ -o lcd -l rt lcd.cpp -l bcm2835

clean:
	rm pianophone
	rm lcd
