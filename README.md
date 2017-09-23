
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

To install Pyland, follow the instructions for either the Raspberry Pi or Desktop Linux. The instructions will guide you through getting the dependencies and then downloading, compiling and running Pyland. The commands given below should be entered into the 'terminal'.

These install instructions may be incomplete and you may require additional packages to the ones listed here.

#### Getting Dependencies (Raspberry Pi)

Install packages from the Raspbian repo:
```
sudo apt-get update
sudo apt-get install --no-install-recommends build-essential libx11-dev libtinyxml-dev zlib1g-dev mesa-common-dev mesa-utils libgl1-mesa-dev libboost-all-dev python3-dev
sudo apt-get install libqscintilla2-dev libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libgoogle-glog-dev sqlite3
```

Get Pyland source code using git:
```bash
sudo apt-get install --no-install-recommends git-core
git clone http://github.com/pyland/pyland
```

#### Compiling (Raspberry Pi)

For compiling on the Raspberry Pi, use:

```bash
#One needs to be in the pyland folder

python3 build.py -c g++

#g++ can be replaced with any C++11 compiler
```

#### Getting Dependencies (Desktop GNU/Linux)

The dependencies for the Raspberry Pi and Desktop are similar. In fact, the same apt-get commands, given above, should work for a Desktop Debian PC as they do for the Pi. If you're running a different Linux distribution, use the list given in the Raspberry Pi section as a guide.

#### Compiling (Desktop GNU/Linux)

For compiling on desktop, use:

```bash
#You must set the current directory in terminal to the 'pyland' folder (that was downloaded from github)

python3 build.py -d -c g++

#g++ can be replaced with any C++11 compiler
#Notice the -d flag. This specifies that you are compiling for desktop.
#The -j flag followed by a number can be used to specify the number of jobs make uses while compiling. (It is not recommended to use this on a Pi, due to memory limitations.)
```

Please note that desktop support is secondary, and may be incomplete. At the moment, there is only a Unix version.

##Running Pyland (All Platforms)

To launch Pyland, set the current directory to the 'pyland' folder and then enter the following commands:

```bash
cd game
./pyland
```

###Troubleshooting

####Game crashes near the beginning (Raspberry Pi)

The Pi's default memory split doesn't give enough memory to the GPU in order to run the game at high resolutions.

You can fix this by running `raspi-config`, going into `Advanced Options`, `Memory Split`, and trying a larger value (e.g. 128). Make sure to reboot for the changes to take effect.

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
