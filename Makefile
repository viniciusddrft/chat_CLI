all:
	gcc Main.c Server.c Client.c -o Main -lpthread
clean:
	rm -f Main
