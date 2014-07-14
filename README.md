Project Zygote (working title)
==============================

This project is pre-alpha, the state of the project changes rapidly from day to day. Follow us on twitter for updates, [@ProjectZygote](twitter.com/ProjectZygote).

## Install 

#### Raspberry Pi

```bash
git clone https://github.com/heidi-ann/zygote
sudo apt-get install libx11-dev gdebi libtinyxml-dev zlib1g-dev mesa-common-dev mesa-utils mesa-utils-extra build-essential
wget http://people.ds.cam.ac.uk/ajn44/files/libsdl2_2.0.3-1_armhf.deb
sudo gdebi libsdl2_2.0.3-1_armhf.deb 
cd zygote/src
COMPILER=g++-4.7 PYTHON_VERSION=3.2 LIBBOOST_PYTHON=boost_python-py32 make
```

#### Desktop

For compiling on desktop, use:
```bash
PLATFORM=desktop COMPILER=g++ ... make
```

Please note that desktop support is secondary, and may be incomplete.
