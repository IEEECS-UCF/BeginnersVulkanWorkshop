# Literally just makes includes easier (don't have to specify 'src' every time)
target_sources(VulkanWorkshop 
	# Source files
	PRIVATE
		main.c

	# Header files
	PRIVATE
		FILE_SET internalHeaders
		TYPE HEADERS
		FILES
			
)

