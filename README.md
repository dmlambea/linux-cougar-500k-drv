# linux-cougar-500k-drv

A very basic but functional Linux driver for the Cougar 500k Gaming Keyboard.

**Note:** although this driver makes the keyboard usable, it is just a workaround driver so that the keyboard no longer freezes when an extended key is pressed. This is not final nor production-quality code. Please read https://bugs.launchpad.net/ubuntu/+source/linux/+bug/1511511 for a description of the bug this driver tries to solve.

# Installation and usage

This driver has been built and tested in Ubuntu Server 18.04 LTS, kernel 4.15.0-23-generic. Please refer to DKMS documentation (e.g., https://help.ubuntu.com/community/DKMS) for instructions on how to build a DKMS module for your distribution.

The `hid_generic` module must be unloaded before plugging in the keyboard. Otherwise, `hid_generic` will take on the device and this module will not have any effect.

# TODO list and known bugs

## TODO

* Handle all extended keys, not only the 8-key sample mapping.
* Make this driver the first option for Cougar 500k devices, not the default `hid_generic`.
* A lot more, for sure.

## Known bugs

* A kernel Oops is raised when the device is disconnected. It happens with the `.remove` function being installed or not.
