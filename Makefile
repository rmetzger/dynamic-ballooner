CC		=	g++
CFLAGS	=	-O3 -march=native -lprocps -Wall 

all: ballooner
	

run: all
	echo "+++++ Running Example: +++++"
	./ballooner
	./ballooner 500

ballooner : main.cpp
	$(CC) $< $(CFLAGS) -o $@