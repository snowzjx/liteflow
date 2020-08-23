EXTRA_CFLAGS := -I$(src)/../include

obj-m += lf_kernel.o
lf_kernel-objs := liteflow.o liteflow_model.o liteflow_nl.o

obj-m += lf_tcp_kernel.o 
lf_tcp_kernel-objs := liteflow_tcp.o

obj-m += lf_netfilter_kernel.o 
lf_netfilter_kernel-objs := liteflow_netfilter.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
			
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
