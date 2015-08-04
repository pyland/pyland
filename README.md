
![alt tag](https://raw.githubusercontent.com/pyland/pyland/master/resources/logo.png)

This project is pre-alpha, the state of the project changes rapidly from day to day. Follow us on twitter for updates, [@ProjectPyland](http://twitter.com/ProjectPyland).

## About

Pyland aims to provide a fun and creative environment on the Raspberry Pi (or
other platforms) to aid children learning programming and Computer Science
Concepts. It was started as a summer intern project at the University of
Cambridge Computer Laboratory June-August 2014 supported by a grant from the
Broadcom Foundation. It was started by Ben Catterall, Heidi Howard, Joshua
Landau, and Ashley Newson under the supervision of Alex Bradbury and Robert
Mullins. It's currently being worked upon by Tom Read-Cutting, Alexander Day and Anindya Sharma.

![Screenshot](https://raw.githubusercontent.com/pyland/pyland/master/resources/screenshot.png)

## Install
These install instructions may be incomplete and you may require additional packages to the ones listed here

#### Getting Dependencies (Raspberry Pi)

Install packages from raspbian repo:
```bash
sudo apt-get update
sudo apt-get install --no-install-recommends libx11-dev gdebi libtinyxml-dev g++-4.7 zlib1g-dev
sudo apt-get install --no-install-recommends mesa-common-dev mesa-utils build-essential gedit
sudo apt-get install --no-install-recommends libboost1.50-all-dev python3.2-dev libgl1-mesa-
sudo apt-get install libts-dev

```

Download and install extra dependancies:
```bash
mkdir pyl-temp
cd pyl-temp
wget http://people.ds.cam.ac.uk/ajn44/files/libsdl2_2.0.3-1_armhf.deb
sudo gdebi libsdl2_2.0.3-1_armhf.deb
wget http://people.ds.cam.ac.uk/ajn44/files/libsdl2-image_2.0.0-1_armhf.deb
sudo gdebi libsdl2-image_2.0.0-1_armhf.deb
wget http://people.ds.cam.ac.uk/ajn44/files/libsdl2-ttf_2.0.12-1_armhf.deb
sudo gdebi libsdl2-ttf_2.0.12-1_armhf.deb
wget http://people.ds.cam.ac.uk/ajn44/files/libgoogle-glog0_0.3.3-1_armhf.deb
sudo gdebi libgoogle-glog0_0.3.3-1_armhf.deb
wget http://people.ds.cam.ac.uk/ajn44/files/libgoogle-glog-dev_0.3.3-1_armhf.deb
sudo gdebi libgoogle-glog-dev_0.3.3-1_armhf.deb
cd ..
```

Get source code using git:
```bash
sudo apt-get install --no-install-recommends git-core
git clone http://github.com/pyland/pyland
```

#### Compiling on Raspberry Pi

For compiling on the Raspberry Pi, use:

```bash
cd pyland
#Put your python version here (change both PYTHON_VERSION and LIBBOOST_PYTHON). Need at least 3.2.
COMPILER=g++-4.7 PYTHON_VERSION=3.2 LIBBOOST_PYTHON=boost_python-py32 make
```

#### Compiling on Desktop - Unix

For desktop, you will need the packages listed above, which can be found here:

```bash
sudo apt-get update
sudo apt-get install --no-install-recommends libx11-dev gdebi libtinyxml-dev g++-4.7 zlib1g-dev
sudo apt-get install --no-install-recommends mesa-common-dev mesa-utils build-essential gedit
sudo apt-get install --no-install-recommends libboost1.55-all-dev python3-dev libgl1-mesa-dev
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-image-dev
sudo apt-get install libsdl2-ttf-dev
sudo apt-get install libgoogle-glog0
sudo apt-get install libgoogle-glog-dev
```

If you have problems installing these packages, you can start entering their names and press tab for the currently available packages.

For compiling on desktop, use:

```bash
# Set both PYTHON_VERSION and LIBBOOST_PYTHON to the version you have installed.
# LIBBOOST_PYTHON is typically boost_python-py34 or boost_python3.
#
# Set COMPILER to the most recent g++ or clang++ version available,
# but at least g++-4.7 or a recent clang++. Plain "g++" is the default.
PLATFORM=desktop COMPILER=g++-4.7 PYTHON_VERSION=3.4 LIBBOOST_PYTHON=boost_python-py34 make
```

Please note that desktop support is secondary, and may be incomplete. At the moment, there is only a Unix version.

##Usage

To launch :

```bash
cd src
./main.bin
```

Keybindings
* <kbd>e</kbd> - open the editor with current sprite's script
* <kbd>r</kbd> - run the script for the current sprite
* <kbd>h</kbd> - stop the script
* <kbd>Shift</kbd> - Hold down when running a script for fast mode (can't use with manual control)

* <kbd>up</kbd><kbd>down</kbd><kbd>left</kbd><kbd>right</kbd> / <kbd>w</kbd><kbd>a</kbd><kbd>s</kbd><kbd>d</kbd> - move sprite
* <kbd>m</kbd> - print the focused sprite's location

* <kbd>escape</kbd> - go back to level selection map

* <kbd>=</kbd><kbd>-</kbd> - zooming in and out
* <kbd>Ctrl</kbd>-<kbd>0</kbd> - return to default zoom level

##Installing qt for integrated editor test

Install the following packages:

```bash
sudo apt-get install libqscintilla2-dev
```

This will create the executable "application" which can then be run.

##API

* `help()` and `help(command)` - Get help on the current task and any in-game (or other) commands.

* `move(direction)` - Move the character in the given direction. Parameter direction: north, east, south or west
* `walkable(direction)` - Determines if the character can move in that direction. Parameter direction: north, east, south or west

* `cut(direction)` - Cuts down vines or logs. Parameter direction: north, east, south or west
* `look(radius)` - Find all objects in a given radius from the character. Parameter: radius of the area to search for objects in.

* `monologue()` - Prints to the screen the name and location of a character.
