# linux-cougar-500k-drv

A functional Linux driver for the Cougar 500k Gaming Keyboard.

This driver solves the bug described here: https://bugs.launchpad.net/ubuntu/+source/linux/+bug/1511511

# Installation and usage

This driver has been built and tested in Ubuntu Server 18.04 LTS, kernel 4.15.0-23-generic. Please refer to DKMS documentation (e.g., https://help.ubuntu.com/community/DKMS) for instructions on how to build a DKMS module for your distribution.

A Docker container can be used to cleanly compile the module and get a **.deb** file from it. The next sections explain how to use the provided Dockerfile to create a build container and how to use it for creating the module install package in **.deb** format.

## Create the Docker build image

Run the following command to create a Docker build image, if you don't have created it yet:

> docker build -t dkmsbuilder .

Where `dkmsbuilder` is the container's image name (you can use your own name, but write it down for later) and `.` is the directory where the file `Dockerfile` resides (this project's root).

## Launch the module builder script

Run:

> ./build_module_package.sh dkmsbuilder

Where `dkmsbuilder` is the container's image name you created before (or the name you specified when creating the image). This script will search for the module's source code in the `./module` directory and will leave the resulting **.deb** file in the directory `./build`.

Note that the container has been designed to build modules as non-root user ('nobody:nogroup', to be precise). Make sure your `./build` directory should be writable by nobody:nogroup, or the creation will fail due to lack of privileges.

## Install the module

Install the **.deb** file as usual. It will create a `cougar_500k` module for your running kernel, as well as udev rules for properly binding the keyboard to it. This rules are required because `hid_generic` module takes on the keyboard. The udev rules unbind the device from `hid_generic`, then rebind it to this module.

# TODO list and known bugs

## TODO

* Make this driver the first option for Cougar 500k devices, not the default `hid_generic`.
* All special keys (macro recording, led preferences and FN key combos) are handled by the hardware itself, so no special handling is required. Anyways, the keys are detected by the module (although it does nothing with them), so it would be good to write some kind of userspace tool to enrich the user experience.
* A lot more, for sure.

# A note to users/maintainers/volunteers

This is my very first implementation of anything related to linux kernel code, so please be patient. Feel free to contact me if you believe I can help you, but don't expect too much: I will try my best.

If you want to enhance/improve this code and you don't have a keyboard to test, I can send you data, keycodes or any information you might need.
