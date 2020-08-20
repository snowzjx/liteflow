EXTRA_CFLAGS := -I$(src)/../include

obj-m += lf_kernel.o
lf_kernel-objs := liteflow.o liteflow_model.o liteflow_nl.o

obj-m += lf_tcp_kernel.o 
lf_tcp_kernel-objs := liteflow_tcp.o

obj-m += lf_firewall_kernel.o 
lf_firewall_kernel-objs := liteflow_firewall.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
			
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
