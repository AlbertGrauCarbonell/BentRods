COMPILERFLAGS = -Wall -g
CC = gcc
CFLAGS = $(COMPILERFLAGS)
LIBRARIES = -lm -ltiff
OBJECTS= tiff_io.o overlaympxfull.o

All: overlay

overlay: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)


$(OBJECTS) : tiff_io.h

overlay.o:

clean:
	rm $(OBJECTS) main

