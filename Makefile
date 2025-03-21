OBJS = main.o server.o http.o file.o http_header.o parse_args.o
UNAME := $(shell uname)
CFLAGS = -g -D_GNU_SOURCE

server: $(OBJS)
	gcc $(OBJS) -o server

%.o: src/%.c
	gcc $(CFLAGS) -c $^ -o $@

clean:
	rm $(OBJS)