#!/bin/bash

insmod lf_model_1.ko

wget https://conferences.sigcomm.org/sigcomm/2022/ -O /tmp/sigcomm1.html

insmod lf_model_2.ko

wget https://conferences.sigcomm.org/sigcomm/2022/ -O /tmp/sigcomm2.html

insmod lf_model_3.ko

wget https://conferences.sigcomm.org/sigcomm/2022/ -O /tmp/sigcomm3.html

dmesg