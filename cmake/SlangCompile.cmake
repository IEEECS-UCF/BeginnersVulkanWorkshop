# Utter geminislop but I cannot be bothered to fix it right now
find_program(SLANGC_EXECUTABLE slangc REQUIRED)

# Compiles Slang shaders located in ${CMAKE_SOURCE_DIR}/shaders
#
# Usage:
#   compile_slang_shaders(
#       TARGET MyGameTarget             # Target that depends on the shaders
#       SOURCES tri.slang other.slang   # Filenames relative to shaders/
#       ENTRYPOINTS vertMain fragMain   # One or more entrypoints
#       [OUTPUT_DIR <path>]             # Optional: defaults to ${CMAKE_BINARY_DIR}/shaders/byte
#   )
function(compile_slang_shaders)
    set(options)
    set(oneValueArgs TARGET OUTPUT_DIR)
    set(multiValueArgs SOURCES ENTRYPOINTS EXTRA_ARGS)
    cmake_parse_arguments(SLANG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT SLANG_TARGET)
        message(FATAL_ERROR "compile_slang_shaders: TARGET must be specified")
    endif()

    if(NOT SLANG_SOURCES)
        message(FATAL_ERROR "compile_slang_shaders: SOURCES must be specified")
    endif()

    if(NOT SLANG_ENTRYPOINTS)
        message(FATAL_ERROR "compile_slang_shaders: At least one ENTRYPOINT must be specified")
    endif()

    # Default output directory in the build tree (keeps source clean)
    if(NOT SLANG_OUTPUT_DIR)
        set(SLANG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders/byte")
    endif()

    # Build entrypoint flags: -entry <ep1> -entry <ep2> ...
    set(ENTRYPOINT_FLAGS)
    foreach(ENTRY ${SLANG_ENTRYPOINTS})
        list(APPEND ENTRYPOINT_FLAGS -entry "${ENTRY}")
    endforeach()

    set(COMPILED_SPV_FILES)

    foreach(SHADER_SRC ${SLANG_SOURCES})
        # Resolve full path to shader in project_root/shaders
        set(INPUT_PATH "${CMAKE_SOURCE_DIR}/shaders/${SHADER_SRC}")

        # Derive output .spv name from base filename
        get_filename_component(SHADER_NAME "${SHADER_SRC}" NAME_WLE)
        set(OUTPUT_SPV "${SLANG_OUTPUT_DIR}/${SHADER_NAME}.spv")

        add_custom_command(
            OUTPUT "${OUTPUT_SPV}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${SLANG_OUTPUT_DIR}"
            COMMAND "${SLANGC_EXECUTABLE}" "${INPUT_PATH}"
                    -target spirv
                    -profile spirv_1_4
                    -emit-spirv-directly
                    -fvk-use-entrypoint-name
                    ${ENTRYPOINT_FLAGS}
                    ${SLANG_EXTRA_ARGS}
                    -o "${OUTPUT_SPV}"
            DEPENDS "${INPUT_PATH}"
            COMMENT "Compiling Slang shader: ${SHADER_SRC} -> ${SHADER_NAME}.spv"
            VERBATIM
        )

        list(APPEND COMPILED_SPV_FILES "${OUTPUT_SPV}")
    endforeach()

    # Create a dedicated target for the compiled shaders and attach it
    set(SHADER_TARGET "${SLANG_TARGET}_shaders")
    if(NOT TARGET "${SHADER_TARGET}")
        add_custom_target("${SHADER_TARGET}" DEPENDS ${COMPILED_SPV_FILES})
        add_dependencies("${SLANG_TARGET}" "${SHADER_TARGET}")
    else()
        add_custom_command(TARGET "${SHADER_TARGET}" POST_BUILD DEPENDS ${COMPILED_SPV_FILES})
    endif()
endfunction()
