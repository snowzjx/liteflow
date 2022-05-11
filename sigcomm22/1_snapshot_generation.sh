#!/bin/bash

# Quantizing the NN
python3 ../bin/lf_quant.py ../data/aurora ../data/aurora/representative_dataset.npy ./aurora_int_quan_model.tflite

## Code transformation
python3 ../bin/lf_generate_snapshot.py ./aurora_int_quan_model.tflite 1 1 ./

## Code transformation - Generate model 2
python3 ../bin/lf_generate_snapshot.py ./aurora_int_quan_model.tflite 2 1 ./

## Code transformation - Generate model 3
python3 ../bin/lf_generate_snapshot.py ./aurora_int_quan_model.tflite 3 1 ./

## Compile into kernel module
make
