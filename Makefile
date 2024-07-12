run: compile
	targets/main
dbg: compile
	gdb targets/main
compile: main.c
	mkdir -p targets
	gcc main.c netcode.c draw_together.c queue.c -o targets/main -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lm -g
