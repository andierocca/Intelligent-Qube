Download the following 5 files, and replace <arch> with "amd64" for 64-bit installations, and "i386" for 32-bit installations. You can check your architechture by typing "arch" in a terminal.

cs2110-tools_1.0-1_<arch>.deb
cs2110-tools-emulator-1.3.0-<arch>.deb
libavcodec53_0.8.7-1ubuntu2_<arch>.deb
libavformat53_0.8.7-1ubuntu2_<arch>.deb
libavutil51_0.8.7-1ubuntu2_<arch>.deb

Navigate to the folder where you downloaded these files, and install the C toolchain and emulator dependencies:

(64-bit)
sudo dpkg -i cs2110-tools_1.0-1_amd64.deb libavcodec53_0.8.7-1ubuntu2_amd64.deb libavformat53_0.8.7-1ubuntu2_amd64.deb libavutil51_0.8.7-1ubuntu2_amd64.deb

(32-bit)
sudo dpkg -i cs2110-tools_1.0-1_i386.deb libavcodec53_0.8.7-1ubuntu2_i386.deb libavformat53_0.8.7-iubuntu2_i386.deb libavutil51_0.8.7-1ubuntu2_i386.deb

Now, install the emulator. Running the first line WILL give you a dependency error, this is okay. Just run the next command and it'll set up the package and grab the dependencies.

(64-bit)
sudo dpkg -i cs2110-tools-emulator-1.3.0-amd64.deb
sudo apt-get install -f

(32-bit)
sudo dpkg -i cs2110-tools-emulator-1.3.0-i386.deb
sudo apt-get install -f

If this is all installed correctly, you should be able to type "vbam" in a terminal, and it'll give you a bunch of options for running the emulator. Otherwise, it'll say that the command was not found.
