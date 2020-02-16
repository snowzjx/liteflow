all:
	gcc -Werror -O3	-o test_tcp_nl  -I../include test_tcp_nl.c $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)
	gcc -Werror -O3	-o test_model_activation  -I../include test_model_activation.c $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)
	@cd kernel; make all;
clean:
	-@rm -rf test_tcp_nl test_model_activation
	@cd kernel; make clean;