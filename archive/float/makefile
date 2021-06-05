OBJS=float.o
EFILE=float
CFILE=float.cpp
CFLAGS=-Wall -g 
CC = gcc
LIBS = -lglib -lgdk -lgtk

$(EFILE): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(EFILE) $(CFLAGS)
	#strip $(EFILE)
	# $(EFILE)

$(OBJS): $(CFILE)
	$(CC) -c $(CFILE) $(CFLAGS)

clean:
	rm -f $(OBJS)

clean_all: 
	rm -f $(OBJS) $(EFILE)
