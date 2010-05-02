
CFLAGS= -Wall -g

OBJECT=v6interface.o main.o

v6interface: $(OBJECT)
	$(CC) -o v6interface $(OBJECT)


%.o: %.c
	$(CC) -c $(CFLAGS) $<

