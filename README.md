# libgcadapter


A library for interfacing with the Wii U GameCube Adapter.


# Usage


Initialize the adapter with `gc_adapter_initialize`, then update the status (`gc_adapter_update`) and poll (`gc_pad_poll`) repeatedly until exit (`gc_adapter_free`).



Full documentation can be found in `DOCUMENTATION`.


# Building on Linux


`make`


You will need libncurses5-dev and libusb-1.0-0-dev to build libgcadapter.


A shared and static library will be created in bin/. 


# Building on Windows


Open `msvc\libgcadapter\libgcadapter.sln` and build the desired target.


# Building on Mac OS X


Mac OS X is not supported.


# Bindings


Bindings for .NET are available at https://github.com/HannesMann/libgcadapter.NET.


Feel free to create your own and send a pull request!


# Thanks to


The Dolphin Emulator project (https://github.com/dolphin-emu/dolphin) for its implementation of the Wii U GameCube adapter, which was used as a reference for implementing libgcadapter.
