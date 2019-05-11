This is a test openFrameworks application written to explore integration with libvlc v4

The environment used to edit and build is Visual Studio Code under Linux Ubuntu 18.04. The project can be ported very easily to Qt Creator if needed.

To compile open the VS workspace and press CTRL-SHIFT-B and use the launch options to run or debug

It is of course possible to build via terminal using make Debug or make Release, then make RunDebug (or RunRelease) to start.

Setup: install of 0.10.1 from nightly builds (the download version won't compile on Linux), then install libvlc version 4, these are the steps I had to follow

git clone https://github.com/videolan/vlc.git
sudo apt install flex
sudo apt install bison
./bootstrap 
sudo apt-get install lua5.2 liblua5.2-dev
sudo apt install liba52-0.7.4-dev
sudo apt install libxcb-xkb-dev
./configure --disable-qt
./make
sudo make install

add "/usr/local/lib" to /etc/ld.so.conf (at the end)
sudo ldconfig

test that vlc is working and using library version 4 from command line

build the SDL based sample code from libvlc developers (included here for convenience in SDL_demo folder) and verify that it can decode and display a video file.

Then... have a look at the openframeworks app and see if you can help :)
