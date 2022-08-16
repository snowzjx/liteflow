INCLUDE = ../include
LIB = ../lib

all:
	@cd kernel; make all;
	gcc -Werror -O3	-o test_dp_notification  -I$(INCLUDE) test_dp_notification.c -L$(LIB) -llf $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)
clean:
	@cd kernel; make clean;