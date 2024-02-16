OBJS = main.o server.o http.o file.o

server: $(OBJS)
	gcc $(OBJS) -o server

%.o: %.c
	gcc -c $^ -o $@

clean:
	rm $(OBJS)