## Project

Now with ImGui!!
capacity up to 16k balls with 60 fps
<img width="1196" height="830" alt="image" src="https://github.com/user-attachments/assets/d6a2dc6c-c589-4b10-af32-f802f96a3842" />


## Build

Install the vulkan sdk on the official website and enable everything except for Volk if you don't want it, then just compile with cmake.

Every other dependency has it's folder named, just install from their specific github repo to there.

### LIMITS

The executable needs to be run in the same folder, example: ./VulkanGLFWProject. Not outside of the scope

The shaders MUST be in the shaders folder, the font Monocraft.ttf MUST be on the external folder, and the executable MUST be on the build folder

If you don't like this and want the executable alone, you can use tools to compile the shaders and the font on a .h header file to be include and fix some code

WARNING: THIS CODE RUNS ON GCC, any msvc or clang run attempt might need corrections

## Platform
WINDOWS
Install Vulkan SDK

Linux 
Install Vulkan libs
Arch Example: sudo pacman -S vulkan-headers vulkan-icd-loader vulkan-tools
