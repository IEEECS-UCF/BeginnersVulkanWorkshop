#ifndef FILE_IO_H
#define FILE_IO_H

#include <stddef.h>
#include <stdint.h>

/// Will read spirv bytecode into a heap-allocated buffer aligned with a uint32_t as required by Vulkan. 
/// Returns NULL when errors occur, with appropriate logging
uint32_t* readShaderBytecode(const char* filename, size_t* size);

#endif
