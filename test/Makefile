INCLUDE = ../include
LIB = ../lib

all:
	gcc -Werror -O3	-o test_activation_model  -I$(INCLUDE) test_activation_model.c -L$(LIB) -llf $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)
	gcc -Werror -O3	-o test_activation_notification  -I$(INCLUDE) test_activation_notification.c -L$(LIB) -llf $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)
	gcc -Werror -O3	-o test_dp_notification  -I$(INCLUDE) test_dp_notification.c -L$(LIB) -llf $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)
	@cd kernel; make all;
clean:
	-@rm -rf test_tcp_nl test_model_activation
	@cd kernel; make clean;