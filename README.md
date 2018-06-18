# linux-cougar-500k-drv

A very basic but functional Linux driver for the Cougar 500k Gaming Keyboard.

**Note:** although this driver makes the keyboard usable, it is just a workaround driver so that the keyboard no longer freezes when an extended key is pressed. This is not final nor production-quality code. Please read https://bugs.launchpad.net/ubuntu/+source/linux/+bug/1511511 for a description of the bug this driver tries to solve.

# Installation and usage

This driver has been built and tested in Ubuntu Server 18.04 LTS, kernel 4.15.0-23-generic. Please refer to DKMS documentation (e.g., https://help.ubuntu.com/community/DKMS) for instructions on how to build a DKMS module for your distribution.

## Create the docker build image

Run the following command to create a Docker build image:

 docker build -t dkmsbuilder .

Where `dkmsbuilder` is the container's image name (you can use your own name) and `.` is the directory where the file `Dockerfile` resides (this project's root).

## Launch the module builder script

Run:

 ./build_module_package.sh dkmsbuilder

Where `dkmsbuilder` should match the container's image name you created before. This script will search for the module's source code in the `./module` directory and will leave the resulting **.deb** file in the directory `./build`.

Note that the container has been created to build modules as non-root user ('nobody:nogroup', to be precise). So your `./build` directory should be writable by nobody:nogroup.

## Install the module

Install the **.deb** file as usual.

The `hid_generic` module must be unloaded before plugging in the keyboard. Otherwise, `hid_generic` will take on the device and this module will not have any effect.

# TODO list and known bugs

## TODO

* Handle all extended keys, not only the 8-key sample mapping.
* Make this driver the first option for Cougar 500k devices, not the default `hid_generic`.
* A lot more, for sure.

## Known bugs

* A kernel Oops is raised when the device is disconnected. It happens with the `.remove` function being installed or not. This will not be a problem if you're using the keyboard without unplugging it, but surely this bug deserves a fix.

# A note to users/maintainers/volunteers

This is my very first implementation of anything related to linux kernel code, so please be patient. I don't know well the USB low-level protocol so I cannot provide any support. Feel free to contact me if you believe I can help you, but don't expect any solution: I will do my best.

If you want to enhance/improve this code and you don't have a keyboard to test, I can send you data, keycodes or any information you might need.
