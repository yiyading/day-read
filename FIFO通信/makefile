readfifo: readfifo.c writefifo writefifo_NONBLOCK readfifo_NONBLOCK
	gcc -Wall -o $@ $<
writefifo: writefifo.c
	gcc -Wall -o $@ $<

writefifo_NONBLOCK: writefifo_NONBLOCK.c
	gcc -Wall -o $@ $<
readfifo_NONBLOCK: readfifo_NONBLOCK.c
	gcc -Wall -o $@ $<
