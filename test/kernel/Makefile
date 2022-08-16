EXTRA_CFLAGS := -I$(src)/../../include

obj-m += lf_sample_model.o 
lf_sample_model-objs := liteflow_sample_model.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
			
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
