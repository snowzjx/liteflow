INCLUDE = ../include

all:
	gcc -c -Werror -O3 -fpic liblf_activate_model.c liblf_activation_notification.c liblf_dp_notification.c -I$(INCLUDE) $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)
	gcc -shared -o liblf.so liblf_activate_model.o liblf_activation_notification.o liblf_dp_notification.o