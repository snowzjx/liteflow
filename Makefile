SHELL := /bin/bash

LF_KERNEL := lf_kernel
LF_TCP_KERNERL := lf_tcp_kernel
LF_NETFILTER_KERNERL := lf_netfilter_kernel

LF_LIB := liblf

SHARED_DIR := /usr/local/lib

.PHONY: test lib

all:
		make clean
		make module
		make lib
		make test

module:
		-@mkdir build
		@cp -Rf datapath build
		@cp -Rf include build
		@cd build/datapath; make all;

lib:	
		-@mkdir build
		@cp -Rf lib build
		@cp -Rf include build
		@cd build/lib; make all;

test:
		-@mkdir build
		@cp -Rf test build
		@cp -Rf include build
		@cd build/test; make all;

module_install:
		@cd build/datapath; sudo insmod $(LF_KERNEL).ko

module_remove:
		sudo rmmod $(LF_KERNEL)

lib_install:
		make lib
		sudo cp build/lib/${LF_LIB}.so ${SHARED_DIR}
		sudo ldconfig

lib_remove:
		sudo rm -rf ${SHARED_DIR}/${LF_LIB}.so
		sudo ldconfig

tcp_kernel_install:
		@cd build/datapath; sudo insmod $(LF_TCP_KERNERL).ko
		sudo sysctl net.ipv4.tcp_congestion_control=lf_tcp_kernel

tcp_kernel_remove:
		sudo sysctl net.ipv4.tcp_congestion_control=cubic
		sudo rmmod $(LF_TCP_KERNERL)

netfilter_kernel_install:
		@cd build/datapath; sudo insmod $(LF_NETFILTER_KERNERL).ko

netfilter_kernel_remove:
		sudo rmmod $(LF_NETFILTER_KERNERL)

clean:
		-@rm -rf build