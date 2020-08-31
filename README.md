# LiteFlow

<img src="https://raw.githubusercontent.com/snowzjx/liteflow/master/doc/liteflow-logo.png" alt="logo" width="200">

LiteFlow is a framework to embed a trained neural network into Linux kernel datapath with predictable performance. It provides API to automatically deploy and compress if needed a neural network trained with TensorFlow (We are supporting more!) in user space to Linux kernel. LiteFlow also provides standard API to make the deployed neural networks to interact with Linux kernel datapath, such as TCP congestion control (We are supporting more). LiteFlow also exposes standard API to user space for collecting data used in online learning.

The key of LiteFlow is to provide predictable performance neural network for Linux kernel datapath without the ease to train and re-train model in user space. LiteFlow uses performance centric model compression techniques to make the trained neural network suitable for Linux kernel, and dynamically load the model into the kernel. LiteFlow also provides model rollback functions to ensure the correctness of datapath.

[![Build Status](https://travis-ci.org/snowzjx/liteflow.svg?branch=master)](https://travis-ci.org/liteflow/liteflow)

## Clone the repo

1. Clone from Github
```
git clone git@github.com:liteflow/liteflow.git
```

## Install Dependencies

1. Install using APT
```
apt install -y build-essential libnl-3-dev libnl-genl-3-dev pkg-config linux-headers-generic python3 python3-pip 
```

2. Install using pip3
```
pip3 install -U pip
pip3 install -r nn-loader/requirements.txt
```

## Building LiteFlow

1. Build LiteFlow

```
make all
```

2. Install kernel module

```
make module_install
```
to install LiteFlow general kernel.

3. Install LiteFlow extensions.

1). If you would like to use LiteFlow with TCP congestion control, you should also install LiteFlow TCP kernel.
```
make tcp_kernel_install
```

2). If you would like to use LiteFlow with packet filtering (netfilter), you should also install LiteFlow Netfilter kernel.
```
make tcp_netfilter_install
```

4. Install user space library

```
make lib_install
```

5. Install python library
```
cd python
sudo python3 setup.py install
```

6. Let's GO!!!
