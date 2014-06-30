zygote
======

Raspberry Pi Summer Project


Dependencies not by default installed on the pi:
  Development:
    libsdl2 (see below)
    libx11-dev
    <add boost/python>
  Runtime:
    libsdl2 (see below)
    <add boost/python>

Installing SDL2 on the Pi:
  cd ~
  wget http://people.ds.cam.ac.uk/ajn44/files/libsdl2_2.0.3-1_armhf.deb
  dpkg -i libsdl2_2.0.3-1_armhf.deb
  

Compiling
  In general, the makefile in the src directory can be used as follows:

    COMPILER=g++ make
  
  For compiling on desktop, use:

    PLATFORM=desktop COMPILER=g++ make

  Please note that desktop support is secondary, and may be incomplete.