Project Pyland
==============

This project is pre-alpha, the state of the project changes rapidly from day to day. Follow us on twitter for updates, [@ProjectPyland](http://twitter.com/ProjectPyland).

## Install 
These install instructions may be incomplete and you may require additional packages to the ones listed here

#### Getting Dependancies (Raspberry Pi)

```bash
git clone https://github.com/pyland/pyland
sudo apt-get install libx11-dev gdebi libtinyxml-dev zlib1g-dev mesa-common-dev mesa-utils mesa-utils-extra build-essential gedit
g++-4.7 libsdl2-image-dev  
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

cd zygote/src
```

#### Compiling on Raspberry Pi

For compiling on the Raspberry Pi, use:
```bash
#Put your python version here (change both PYTHON_VERSION and LIBBOOST_PYTHON). Need at least 3.2.
COMPILER=g++-4.7 PYTHON_VERSION=3.2 LIBBOOST_PYTHON=boost_python-py32 make
```

#### Compiling on Desktop - Unix

For desktop, you will need the packages listed above, many of which can be found on the repos e.g. 
```bash
sudo apt-get install libsdl2-ttf-dev 
```

For compiling on desktop, use:
```bash
# Set both PYTHON_VERSION and LIBBOOST_PYTHON.
# LIBBOOST_PYTHON is typically boost_python-py32 or boost_python3.
#
# Set COMPILER to the most recent g++ or clang++ version available,
# but at least g++-4.7 or a recent clang++. Plain "g++" is the default.
PLATFORM=desktop COMPILER=g++-4.7 PYTHON_VERSION=3.2 LIBBOOST_PYTHON=boost_python-py32 make
```

Please note that desktop support is secondary, and may be incomplete. At the moment, there is only a Unix version.

##Usage

Keybindings
* n - spawn new spirte
* r - run script for sprite
* k - kill sprite
* h - halt current script
* m - print the focus sprites location
* arrow keys / wasd - move sprite
* numbers - move focus to sprite with associated number, numbers are allocated sequentially from 1
