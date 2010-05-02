

GST_LIBS=$(shell pkg-config --libs gstreamer-0.10)
GST_INCLUDE=$(shell pkg-config --cflags gstreamer-0.10)

CFLAGS=-Wall $(GST_INCLUDE)

all: test oggplayer spectrum2


test: test.o
	$(CC) -o test test.o $(GST_LIBS)

oggplayer: oggplayer.o
	$(CC) -o oggplayer oggplayer.o $(GST_LIBS)

spectrum: spectrum.c
	$(CC) -Wall -g spectrum.c -o spectrum -D NO_ITERATE `pkg-config gstreamer-0.10 glib-2.0 gtk+-2.0 --cflags --libs`

%.o: %.c
	$(CC) $(CFLAGS) -c $<

spectrum2: spectrum2.o
	$(CC) -o spectrum2 spectrum2.o $(GST_LIBS)


