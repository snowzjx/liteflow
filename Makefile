SHELL := /bin/bash

MODULENAME := liteflow
SAMPLENAME := lf_sample

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

sample_install:
		make module;
		@cd build/datapath; sudo insmod $(SAMPLENAME).ko

sample_remove:
		sudo rmmod $(SAMPLENAME)

clean:
		-@rm -rf build
