cmake_minimum_required(VERSION 4.4.3)

project(VulkanWorkshop)


# If using Clangd, will play nice
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
# GLFW building args, needed to not explode the build time
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


# Automagically setup Vulkan
find_package(Vulkan REQUIRED)
# Automagic build of GLFW
add_subdirectory(libs/GLFW)


add_executable(VulkanWorkshop)


# Folds in source files for building,
# adding new files must be done at the 'src' level CMakeLists
add_subdirectory(src)


target_include_directories(VulkanWorkshop 
	PRIVATE 
		${Vulkan_INCLUDE_DIRS}
		libs/GLFW/include
)

target_link_libraries(VulkanWorkshop
	PRIVATE
		Vulkan::Vulkan
		glfw
)
