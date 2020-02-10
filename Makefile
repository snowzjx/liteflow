SHELL := /bin/bash

MODULENAME := liteflow

module:
		-@mkdir build
		@cp -Rf datapath build
		@cp -Rf include build
		@cd build/datapath; make all;

module_install:
		make module;
		@cd build/datapath; sudo insmod $(MODULENAME).ko

module_remove:
		sudo rmmod $(MODULENAME)

clean:
		-@rm -rf build
