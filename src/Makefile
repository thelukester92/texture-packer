CFLAGS=-std=c++11 $(shell libpng-config --cflags)
LFLAGS=$(shell libpng-config --ldflags)
OUTPUT=../bin/texture_packer

all:
	g++ texture_packer.cpp -o $(OUTPUT) $(CFLAGS) $(LFLAGS)
