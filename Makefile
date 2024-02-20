OBJS = main.o server.o http.o file.o http_header.o

server: $(OBJS)
	gcc $(OBJS) -o server

%.o: src/%.c
	gcc -c $^ -o $@

clean:
	rm $(OBJS)