# Literally just makes includes easier (don't have to specify 'src' every time)
target_sources(VulkanWorkshop 
	# Source files
	PRIVATE
		main.c
		helpers/log.c
		helpers/window.c

	# Header files
	PRIVATE
		FILE_SET internalHeaders
		TYPE HEADERS
		FILES
			helpers/log.h
			helpers/window.h
)

