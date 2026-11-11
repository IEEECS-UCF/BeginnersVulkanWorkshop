slangc shaders/tri.slang \
	-target spirv \
	-profile spirv_1_4 \
	-emit-spirv-directly \
	-fvk-use-entrypoint-name \
	-entry vertMain \
	-entry fragMain \
	-o shaders/byte/tri.spv
