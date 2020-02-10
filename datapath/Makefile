EXTRA_CFLAGS := -I$(src)/../include

obj-m += lf_sample.o
lf_sample-objs := model_container.o sample_model.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
			
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
