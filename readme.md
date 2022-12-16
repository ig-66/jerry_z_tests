
# JerryScript for Zephyr, JerryZ

The JerryZ is a Zephyr compatible JavaScript engine, made from a modified JerryScript build process. The JerryZ repo can be found in this [repo](https://github.com/moduhub/zephyr_jerryscript). 

This repository contains tests of the JerryScript in Zephyr, AKA JerryZ, follow the instruction for installation.

# Installation
On an empty folder, run the command:
```
west init -m https://github.com/ig-66/jerry_z_tests
```
This command will result in the following:
```
[folder_where_the_command_was_executed]
    |
    + .west 
    |
    + jerry_z_tests
        |
        + west.yml
        |
        + ...
``` 
Now run:
```
west update
```
# Running the Examples
## Selecting the Example to Build
To select the example simply run the `menuconfig` or `guiconfig`:
```
west build -b <board> -t guiconfig
```
## Flashing the Build
Simply run:
```
west flash
```
