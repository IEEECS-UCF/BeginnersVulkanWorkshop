# Literally just makes includes easier (don't have to specify 'src' every time)
target_sources(VulkanWorkshop 
	# Source files
	PRIVATE
		main.c
		helpers/log.c
		helpers/window.c
		helpers/vulkanDebug.c
		instance.c
		physicalDevice.c
		helpers/searching.c
		logicalDevice.c
		swapchain.c

	# Header files
	PRIVATE
		FILE_SET internalHeaders
		TYPE HEADERS
		FILES
			helpers/log.h
			helpers/window.h
			helpers/vulkanDebug.h
			instance.h
			physicalDevice.h
			helpers/searching.h
			logicalDevice.h
			swapchain.h
)

