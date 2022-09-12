all:
	@echo "--- START BUILD ---"
	gcc Main.c Server.c Client.c -o Main -lpthread
	@echo "--- DONE BUILD ---"
clean:
	@echo "--- START CLEAN ---"
	rm -f Main
	@echo "--- DONE CLEAN ---"