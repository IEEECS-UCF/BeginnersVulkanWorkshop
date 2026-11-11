# A Short Tour of Vulkan
## What is this?
"A Short Tour of Vulkan" is a simple introduction to the Vulkan GPU API hosted by IEEE CS through our ongoing workshops for students of UCF. The C code present is an accessory to the slideshow contained within this repository as an activity to do while the presentation is ongoing, providing a way to get hands on with the tools at play. This should not be used as a proper way to setup a renderer or even modern way to write a Vulkan powered application, but rather an opinionated toe-dip.
  
> [!IMPORTANT] 
> This is designed for members of IEEE CS @ UCF and ongoing maintence will not be performed outside of that dictated by the officers of the club. If you're a student at UCF and want to know more, we maintain a presence on campus best we can, and are open to contact through our Discord or in person

## Project Setup
To work with this project, you'll need four major things (which will be elaborated on individually soon):
1. __A working version of git__ to pull in the required code for you (this repo and other supporters)
2. __A C compiler supporting C99 or above__, GCC and Clang have been tested against this project and both compile properly
3. An IDE or other form of workspace that allows for leveraging __the CMake toolchain__
4. A working version of __the [version 1.4] Vulkan SDK__, including the Slang shading language compiler

### Git    
If you do not already have git installed on your system, now is an important time to do so.  
__To work with this project, run these series of commands (in a terminal) to get everything you'll need code wise__
```
git clone https://github.com/IEEECS-UCF/BeginnersVulkanWorkshop.git VulkanWorkshop
cd VulkanWorkshop
git submodule update --init --recursive
```
__From this you can open the project in your editor of choice (do refrence the CMake section though)__  
If you care, the commands do, in order: create a local copy of this project in the folder/directory 'VulkanWorkshop', go into that directory, and finally pull in additional projects (GLFW in this case) automagically.

### C Compiler
Some form of on device C compiler will be __required__ for this workshop, an online one is not adequate. I highly reccomend either GCC or Clang.
#### Windows
Windows has its own C compiler MSVC, but due to a myriad of reasons it has not been tested for compatability with this workshop. My preffered method of aquiring a C compiler on windows is through a tool called [MSYS2](https://www.msys2.org), which will allow local running of the commands (WSL2 is not an option here). Many an online tutorial exists for installing it and a compiler, so find whichever takes your fancy.
#### Linux
Both compilers are likely available through your distriubutions preffered package manager, please consult it's documentation on proper installation.

### CMake
CMake is a tool for organizing the compiling of large C/C++ projects, install instructions are similar to that of your C compiler, [MSYS2](https://www.msys2.org/docs/cmake/) for Windows, package manager for Linux.  
__Needed commands are here (if your editor environemnt doesn't handle CMake itself), at the directory where you git clone'd__
```
mkdir build
cmake -G [your installed generator] -DCMAKE_BUILD_DEBUG -B build
```
If you want to take it on a test ride (which I reccomend to make sure everyhting has worked [Install Vulkan first!]), do this
```
cmake --build build
./build/VulkanWorkshop
```
And it should run no problem!

### Vulkan SDK
LunarG provides the Vulkan SDK for the three major platforms, Windows, Linux, and MacOS. We have confirmed this project working in the first two, but were unable to verify MacOS having a smooth experience (but if you can get it to work awesome!). Follow their download instructions here [https://vulkan.lunarg.com/doc/view/latest/windows/getting_started.html] and the download page itself here [https://vulkan.lunarg.com/sdk/home]. Follow the instructions for your OS, and make sure to have both Validation Layers and the Slang compiler installed. The internet, chatbots, and if a UCF student the running officer can help if issues occur.

--- 
*This work was originally compiled and presented by officer Harrison Hilpert*