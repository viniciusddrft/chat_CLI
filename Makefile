all:
	gcc -o main main.c -lpthread
clean:
	rm -f main
