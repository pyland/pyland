Project Zygote (working title)
==============================

This project is pre-alpha, the state of the project changes rapidly from day to day. Follow us on twitter for updates, [@ProjectZygote](twitter.com/ProjectZygote).

## Install 
May be incomplete and you may require additional packages to the ones listed here

#### Raspberry Pi and Desktop installation

```bash
git clone https://github.com/heidi-ann/zygote
sudo apt-get install libx11-dev gdebi libtinyxml-dev zlib1g-dev mesa-common-dev mesa-utils mesa-utils-extra build-essential gedit
g++-4.7 libsdl2-image-dev
wget http://people.ds.cam.ac.uk/ajn44/files/libsdl2_2.0.3-1_armhf.deb
sudo gdebi libsdl2_2.0.3-1_armhf.deb
wget http://people.ds.cam.ac.uk/ajn44/files/libsdl2-image_2.0.0-1_armhf.deb
sudo gdebi libsdl2-image_2.0.0-1_armhf.deb
cd zygote/src
```

#### Compiling on Raspberry Pi

For compiling on the Raspberry Pi, use:
```bash
#Put your python version here (change both PYTHON_VERSION and LIBBOOST_PYTHON). Need at least 3.2.
COMPILER=g++-4.7 PYTHON_VERSION=3.2 LIBBOOST_PYTHON=boost_python-py32 make
```

#### Desktop - Unix

For compiling on desktop, use:
```bash
#Put your python version here (change both PYTHON_VERSION and LIBBOOST_PYTHON). Need at least 3.2.
PLATFORM=desktop COMPILER=g++-4.7 PYTHON_VERSION=3.2 LIBBOOST_PYTHON=boost_python-py32 make
```

Please note that desktop support is secondary, and may be incomplete. At the moment, there is only a Unix version.
