EXTRA_CFLAGS := -I$(src)/../include

obj-m += lf_model_1.o lf_model_2.o lf_model_3.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
			
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
