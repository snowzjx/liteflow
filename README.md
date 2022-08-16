<div align="center">
<picture>
  <img alt="LiteFlow Logo" src="https://raw.githubusercontent.com/snowzjx/liteflow/master/doc/liteflow-logo.png" alt="logo" width="200">
</picture>
<h1>A Hybrid Framework to Build High-performance Adaptive Neural Networks for Kernel Datapath</h1>
</div>

[![Build Status](https://travis-ci.org/snowzjx/liteflow.svg?branch=master)](https://travis-ci.org/liteflow/liteflow)

Please refer to our [SIGCOMM '22 paper](https://dl.acm.org/doi/10.1145/3544216.3544229) for details.

**ATTENTION:** This project is for research purposes and please do not use it in a production environment. **Some of the code is not opensourced due to compatibility issues now and we are working on it to release them soon.** However, you can still experience some of the key components mentioned in our paper.

**Tested Kernel Version:** 4.15.0-173-generic

## Getting Started

### Installation

1. Clone the repo

    ```sh
    git clone https://github.com/snowzjx/liteflow.git
    ```

1. Change the directory to LiteFlow

    ```sh
    cd liteflow
    ```

1. Install required packages

    ```sh
    apt install -y build-essential libnl-3-dev libnl-genl-3-dev pkg-config linux-headers-generic python3 python3-pip 
    ```

    ```sh
    pip3 install -U pip
    pip3 install -r bin/requirements.txt
    ```

1. Compile

    ```sh
    make all
    ```

1. Install LiteFlow kernel module

    ```sh
    make module_install
    ```

    You can check if the LiteFlow kernel module is successfully installed by executing ```lsmod``` to see if *lf_kernel* is there.

1. Install TCP Congestion Control module

    ```sh
    make tcp_kernel_install
    ```

    You can check if the LiteFlow TCP Congestion Control is successfully installed by executing ```lsmod``` to see if *lf_tcp_kernel* is there.

    You can also check if the TCP Congestion Control is successfully activated by executing ```sysctl net.ipv4.tcp_congestion_control``` to see whether is value is *lf_tcp_kernel*

### Examples

#### Snapshot Generation

```sh
cd script
./snapshot_generation.sh
```

This command tries to generate 3 Aurora snapshots from the model in ```liteflow/data/```. You can verify if the command is successful by checking if *lf_model_1.ko*, *lf_model_2.ko*, and *lf_model_3.ko* are generated. You can also check the generated source code by looking into *lf_model_1.c*, *lf_model_2.c*, and *lf_model_3.ko* files. Note here, we generate 3 identical snapshots just for demo purposes.

#### Snapshot Update

```sh
cd script
./active_standby_switch.sh
```

You expect a *dmesg* output showing that LiteFlow uses different neural networks to serve different flows.

```sh
[ 2480.330440] Current slot 0 is registered with model: -1
[ 2480.330440] Current slot 1 is registered with model: 1
[ 2480.330440] Current active slot is: 1
...
[ 2482.826293] Using model with uuid: 1 for inference...
...
[ 2483.578519] Current slot 0 is registered with model: 2
[ 2483.578519] Current slot 1 is registered with model: 1
[ 2483.578519] Current active slot is: 0
...
[ 2483.823231] Using model with uuid: 2 for inference...
...
[ 2484.844727] Current slot 0 is registered with model: 2
[ 2484.844727] Current slot 1 is registered with model: 3
[ 2484.844727] Current active slot is: 1
...
[ 2485.099799] Using model with uuid: 3 for inference...
...
```

#### Batched Training Data Delivery

```sh
cd script
./fetch_data_from_kernel.sh
```

The screen should print all training data. *Note: different kernel settings may cause different outputs, please change the code accordingly.*

#### LiteFlow Userspace Demo

```sh
cd bin
python3 lf.py
```

You expect to see a demo output of how LiteFlow generates a snapshot, evaluates a new neural network with the existing snapshot and updates the snapshot. *Note: for compatibility issues, there are lots of places left blank in the userspace program of LiteFlow, users can fill in the blanks with their own needs...*

## Citation

```bibtex
@inproceedings{liteflow, 
    author      = {Junxue Zhang and Chaoliang Zeng and Hong Zhang and Shuihai Hu and Kai Chen}, 
    title       = {LiteFlow: Towards High-Performance Adaptive Neural Networks for Kernel Datapath}, 
    year        = {2022}, 
    booktitle   = {Proceedings of the ACM SIGCOMM 2022 Conference}, 
}
```
