CC=gcc
CFLAGS=-Wall -Wextra -Iinclude

SRCS=src/main.c src/http.c src/handlers.c
OBJS=$(SRCS:.c=.o)
EXEC=cserve

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)