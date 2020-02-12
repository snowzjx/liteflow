SHELL := /bin/bash

LF_KERNEL := lf_kernel
LF_TCP_KERNERL := lf_tcp_kernel
SAMPLEMODEL := lf_sample_model

module:
		-@mkdir build
		@cp -Rf datapath build
		@cp -Rf include build
		@cd build/datapath; make all;

module_install:
		@cd build/datapath; sudo insmod $(LF_KERNEL).ko

module_remove:
		sudo rmmod $(LF_KERNEL)

tcp_kernel_install:
		@cd build/datapath; sudo insmod $(LF_TCP_KERNERL).ko
		sudo sysctl net.ipv4.tcp_congestion_control=lf_tcp_kernel

tcp_kernel_remove:
		sudo sysctl net.ipv4.tcp_congestion_control=cubic
		sudo rmmod $(LF_TCP_KERNERL)

sample_model_install:
		@cd build/datapath; sudo insmod $(SAMPLEMODEL).ko

sample_model_remove:
		sudo rmmod $(SAMPLEMODEL)

clean:
		-@rm -rf build
