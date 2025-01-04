app = main

all: 
	gcc -o $(app) $(app).c
run: all
	./$(app)
clean:
	rm -r $(app)
