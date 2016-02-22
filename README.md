# libgcadapter


A library for interfacing with the Wii U GameCube Adapter.


# Usage


Initialize the adapter with `gc_adapter_initialize`, then update the status (`gc_adapter_update`) and poll (`gc_pad_poll`) repeatedly until exit (`gc_adapter_free`).


It is advised to run `gc_pad_poll` on a separate thread to maximize performance.


Full documentation can be found in `DOCUMENTATION.md`.


# Building on Linux


`make`


You will need libncurses5-dev and libusb-1.0-0-dev to build libgcadapter.


A shared and static library will be created in bin/. 


# Building on Windows


Coming soon!


# Building on Mac OS X


Mac OS X is not supported.


# Thanks to


The Dolphin Emulator project (https://github.com/dolphin-emu/dolphin) for its implementation of the Wii U GameCube adapter, which was used as a reference for implementing libgcadapter.
