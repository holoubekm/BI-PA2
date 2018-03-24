#CC=gcc
#CFLAGS = -c -std=c++11 -Wall -pedantic -pg -ggdb -O0 
#CFLAGS = -c -std=c++11 -Wall -pedantic -O3 
#CFLAGS = -std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb -lstdc++

CC=g++
CFLAGS = -std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb

DIR = ./src/
OBJECTS = iplayer.o cdesk.o cgame.o cwindow.o cgui.o cnetwork.o chuman.o cbot.o caiminimax.o cclient.o cserver.o caisimple.o

all: compile doc

compile: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(DIR)main.cpp -lcurses -o holoumar

%.o: $(DIR)%.cpp $(DIR)%.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o holoumar ./doc

doc:
	doxygen Doxyfile

run: compile
	./holoumar
