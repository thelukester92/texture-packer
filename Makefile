CFLAGS=$(shell libpng-config --cflags)
LFLAGS=$(shell libpng-config --ldflags)
OUTPUT=texture_packer.exe

all:
	g++ texture_packer.cpp -o $(OUTPUT) -std=c++11 $(CFLAGS) $(LFLAGS)
