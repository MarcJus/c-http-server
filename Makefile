OBJS = main.o

server: $(OBJS)
	gcc $(OBJS) -o server

%.o: %.c
	gcc -c $^ -o $@

clean:
	rm $(OBJS)