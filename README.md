# CppND-System-Monitor

This project requires the CMake and ncurses library

## To setup and compile in an Ubuntu workspace:

1. Install the `ncurses` package
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```
and if not alrady installed, install `CMake` using the following command
```
sudo apt-get -y install cmake
```
2. Make a build directory and build using cmake
```
mkdir build && cd build
cmake ..
make
```
3. Then from the build folder, run your executable
```
 ./system_monitor
 ```
You should get an output on the terminal showing something like this
<p align="center">
  <img src = "https://github.com/macvincent/CppND-System-Monitor/blob/master/demo.png">
</p>
