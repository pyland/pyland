
![alt tag](https://raw.githubusercontent.com/pyland/pyland/master/small_logo.png)

This project is pre-alpha, the state of the project changes rapidly from day to day. Follow us on twitter for updates, [@ProjectPyland](http://twitter.com/ProjectPyland).

## About

Pyland aims to provide a fun and creative environment on the Raspberry Pi (or
other platforms) to aid children learning programming and Computer Science
Concepts. It was started as a summer intern project at the University of
Cambridge Computer Laboratory June-August 2014 supported by a grant from the
Broadcom Foundation. It was started by Ben Catterall, Heidi Howard, Joshua
Landau, and Ashley Newson under the supervision of Alex Bradbury and Robert
Mullins. It's currently being worked upon by Tom Read-Cutting, Alexander Day and Anindya Sharma.

![Screenshot](https://raw.githubusercontent.com/pyland/pyland/master/screenshot.png)

## Install
These install instructions may be incomplete and you may require additional packages to the ones listed here

#### Getting Dependencies (Raspberry Pi)

Install packages from raspbian repo:
```
sudo apt-get update
sudo apt-get install --no-install-recommends libx11-dev gdebi libtinyxml-dev g++-4.7 zlib1g-dev
sudo apt-get install --no-install-recommends mesa-common-dev mesa-utils build-essential gedit
sudo apt-get install libts-dev
sudo apt-get install --no-install-recommends libboost1.50-all-dev python3.2-dev libgl1-mesa-dev
sudo apt-get install libqscintilla2-dev
sudo apt-get install install sqlite3
```

Download and install extra dependencies:
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
Also requires libsdl2-mixer-dev, not currently on the raspbian repo.

Get source code using git:
```bash
sudo apt-get install --no-install-recommends git-core
git clone http://github.com/pyland/pyland
```

#### Compiling on Raspberry Pi

For compiling on the Raspberry Pi, use:

```bash
#One needs to be in the pyland folder

python3.2 build.py -c g++

#Replace 3.2 with the version of python on the Pi
#g++ can be replaced with any C++11 compiler
#The -j flag followed by a number can be used to specify the number of jobs make uses while compiling
```

#### Getting dependencies on Desktop GNU/Linux
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
sudo apt-get install libqscintilla2-dev
sudo apt-get install install sqlite3
sudo apt-get install libsdl2-mixer-dev 
```

If you have problems installing these packages, you can start entering their names and press tab for the currently available packages.

#### Getting Pyland source code

Get source code using git:

```bash
sudo apt-get install --no-install-recommends git-core
git clone http://github.com/pyland/pyland
```


#### Compiling on Desktop - Unix

For compiling on desktop, use:

```bash
#One needs to be in the pyland folder

python3.2 build.py -d -c g++

#Replace 3.2 with the version of python on the Pi
#g++ can be replaced with any C++11 compiler
#The -j flag followed by a number can be used to specify the number of jobs make uses while compiling
```

Please note that desktop support is secondary, and may be incomplete. At the moment, there is only a Unix version.

##Usage

To launch:

```bash
cd game
./pyland
```

##Keybindings
* <kbd>up</kbd><kbd>down</kbd><kbd>left</kbd><kbd>right</kbd> / <kbd>w</kbd><kbd>a</kbd><kbd>s</kbd><kbd>d</kbd> - Move sprite/Toggle between options
* <kbd>Enter</kbd> - Interact with an object on the map/Proceed through dialogue/Select an option
* <kbd>1-9</kbd> - Run the script of given number/halt running script
* <kbd>Space</kbd> - Run the current script/halt running script
* <kbd>Tab</kbd> - Switch player
* <kbd>Shift</kbd> - Toggle the speed of running scripts

##API

* `face_east(), face_west(), face_south(), face_north()` - Make the character face in the given direction. 
* `move_east(), move_west(), move_south(), move_north()` - Move the character in the given direction. 
* `can_move()` - Determines if the character can move one step ahead.
* `get_position()` - Returns a pair with the x and y coordinates of the character.
* `cut()` - Cuts down vines that are present in front of the character.
* `get_cuts_left()` - Returns an integer, the number of cuts the character has remaining.
* `yell()` - Yells in the given direction. Any object along the line of yelling responds to the yell depending on its type.
* `turn_left(), turn_right()` - Turns the character by 90 degrees anticlockwise and clockwise respectively.

##Creating Levels

To learn how to create your own levels to play in Pyland, you can use the following resources.

Wiki documentation:
https://github.com/pyland/pyland/wiki/Tutorial:----How-to-create-a-simple-level

Video Tutorial series:
https://www.youtube.com/playlist?list=PLnlsbuD48Ge2a5WO1etHQDCsiD-JvUUka
