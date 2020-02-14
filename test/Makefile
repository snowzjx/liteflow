all:
	gcc -Werror -O3	-o test_tcp_nl  -I../include test_tcp_nl.c $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)

clean:
	-@rm -rf test_tcp_nl