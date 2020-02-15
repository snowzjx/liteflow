EXTRA_CFLAGS := -I$(src)/../include

obj-m += lf_kernel.o
lf_kernel-objs := liteflow.o liteflow_model.o liteflow_nl.o

obj-m += lf_tcp_kernel.o 
lf_tcp_kernel-objs := liteflow_tcp.o

obj-m += lf_sample_model.o 
lf_sample_model-objs := liteflow_sample_model.o

# obj-m += lf_sample.o
# lf_sample-objs := model_container.o sample_model.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
			
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
