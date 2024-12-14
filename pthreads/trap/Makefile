
CC=gcc
CFLAGS= -Wall -g
LDFLAGS= -lpthread

#Sorry for the bad make file practices, but it does work 

trap_busy: trap_busy.c trap_mutex
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

trap_mutex: trap_mutex.c trap_sem
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

trap_sem: trap_sem.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)


