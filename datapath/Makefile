EXTRA_CFLAGS := -I$(src)/../include

KERNEL_VERSION_MINOR := $(shell uname -r | awk -F '.' '{print $$2}')
EXTRA_CFLAGS += -D__KERNEL_VERSION_MINOR__=$(KERNEL_VERSION_MINOR)

obj-m += lf_kernel.o
lf_kernel-objs := liteflow.o liteflow_model.o liteflow_nl.o

obj-m += lf_tcp_kernel.o 
lf_tcp_kernel-objs := liteflow_tcp.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
			
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
