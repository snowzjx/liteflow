all:
	gcc -c -Werror -O3 -fpic liblf_activate_model.c liblf_activation_notification.c -I../include $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)
	gcc -shared -o liblf.so liblf_activate_model.o liblf_activation_notification.o
	# gcc -Werror -O3	-o liblf_activation_notification  -I../include liblf_activation_notification.c $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)
	# gcc -Werror -O3	-o liblf_activate_model  -I../include liblf_activate_model.c $(shell pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)