all: folders server client

server: bin/monitor

client: bin/tracer

folders:
	@mkdir -p src obj bin tmp

bin/monitor: obj/monitor.o obj/hashTable.o
	gcc -g obj/monitor.o obj/hashTable.o -o bin/monitor

obj/monitor.o: src/monitor.c src/execute.h src/hashTable.h
	gcc -Wall -g -c src/monitor.c -o obj/monitor.o

obj/hashTable.o: src/hashTable.c src/hashTable.h
	gcc -Wall -g -c src/hashTable.c -o obj/hashTable.o

bin/tracer: obj/tracer.o
	gcc -g obj/tracer.o -o bin/tracer

obj/tracer.o: src/tracer.c src/execute.h
	gcc -Wall -g -c src/tracer.c -o obj/tracer.o

clear:
	rm -f obj/*.o bin/monitor bin/tracer tmp/*