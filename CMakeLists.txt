cmake_minimum_required( VERSION 3.5 )
project( libtitanium LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# TODO: need to respect TITANIUM_SDL=0 in this, atm we assume sdl when we possibly shouldn't
# also TODO: some of these modules suck and should die! i.e. libtitanium_sys, libtitanium_imguiwidgets

add_library( libtitanium_util STATIC )
target_include_directories( libtitanium_util PUBLIC
    ${PROJECT_SOURCE_DIR}
)
target_sources( libtitanium_util PRIVATE
    ${PROJECT_SOURCE_DIR}/libtitanium/util/maths.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/util/commandline.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/util/string.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/util/stacktrace.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/util/data/span_dynamic_tests.cpp
)
target_link_libraries( libtitanium_util PRIVATE libtitanium_memory )
target_compile_options( libtitanium_util PRIVATE ${TITANIUM_COMPILER_ARGS} )


add_library( libtitanium_memory STATIC )
target_include_directories( libtitanium_memory PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${SDL2_INCLUDE_DIRS}
)
target_sources( libtitanium_memory PRIVATE
    ${PROJECT_SOURCE_DIR}/libtitanium/memory/mem_core.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/memory/mem_external.cpp
)
target_link_libraries( libtitanium_memory PRIVATE libtitanium_util )
target_compile_options( libtitanium_memory PRIVATE ${TITANIUM_COMPILER_ARGS} )


add_library( libtitanium_dev STATIC )
target_include_directories( libtitanium_dev PUBLIC
    ${PROJECT_SOURCE_DIR}
)
target_sources( libtitanium_dev PRIVATE
    ${PROJECT_SOURCE_DIR}/libtitanium/dev/tests.cpp
)
target_link_libraries( libtitanium_dev PRIVATE libtitanium_util libtitanium_logger )
target_compile_options( libtitanium_dev PRIVATE ${TITANIUM_COMPILER_ARGS} )

add_library( libtitanium_config STATIC )
target_include_directories( libtitanium_config PUBLIC
    ${PROJECT_SOURCE_DIR}
)
target_sources( libtitanium_config PRIVATE
    ${PROJECT_SOURCE_DIR}/libtitanium/config/config.cpp
)
target_link_libraries( libtitanium_config PRIVATE libtitanium_util )
target_compile_options( libtitanium_config PRIVATE ${TITANIUM_COMPILER_ARGS} )


add_library( libtitanium_logger STATIC )
target_include_directories( libtitanium_logger PUBLIC
    ${PROJECT_SOURCE_DIR}
)
target_sources( libtitanium_logger PRIVATE
    ${PROJECT_SOURCE_DIR}/libtitanium/logger/logger.cpp
)
target_compile_options( libtitanium_logger PRIVATE ${TITANIUM_COMPILER_ARGS} )


add_library( libtitanium_sys STATIC )
target_include_directories( libtitanium_sys PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${SDL2_INCLUDE_DIRS}
)
target_sources( libtitanium_sys PRIVATE
    ${PROJECT_SOURCE_DIR}/libtitanium/sys/platform_sdl.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/sys/sdl_stringify.cpp
)
target_link_libraries( libtitanium_sys PRIVATE libtitanium_util libtitanium_logger ${SDL2_LIBRARIES} )
target_compile_options( libtitanium_sys PRIVATE ${TITANIUM_COMPILER_ARGS} )


add_library( libtitanium_renderer STATIC )
target_include_directories( libtitanium_renderer PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/libtitanium/renderer/extern
    ${SDL2_INCLUDE_DIRS}
)
target_sources( libtitanium_renderer PRIVATE
    ${PROJECT_SOURCE_DIR}/libtitanium/renderer/renderer_main.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/renderer/renderer_device.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/renderer/renderer_target.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/renderer/renderer_command.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/renderer/renderer_stringify.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/renderer/renderer_helpers.cpp
)
target_link_libraries( libtitanium_renderer PRIVATE libtitanium_util libtitanium_config libtitanium_logger
    ${PROJECT_SOURCE_DIR}/libtitanium/renderer/extern/webgpu/libwgpu_native.a
    ${SDL2_LIBRARIES}
)
target_compile_options( libtitanium_renderer PRIVATE ${TITANIUM_COMPILER_ARGS} )


add_library( libtitanium_input STATIC )
target_include_directories( libtitanium_input PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${SDL2_INCLUDE_DIRS}
)
target_sources( libtitanium_input PRIVATE
    ${PROJECT_SOURCE_DIR}/libtitanium/input/input_device.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/input/input_device_enum.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/input/input_actions.cpp
)
target_link_libraries( libtitanium_input PRIVATE ${SDL2_LIBRARIES} )
target_compile_options( libtitanium_input PRIVATE ${TITANIUM_COMPILER_ARGS} )


add_library( libtitanium_resource STATIC )
target_include_directories( libtitanium_resource PUBLIC
    ${PROJECT_SOURCE_DIR}
)
target_sources( libtitanium_resource PRIVATE
    ${PROJECT_SOURCE_DIR}/libtitanium/resource/resource_resolve.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/resource/resource_util.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/resource/readbuf.cpp
    ${PROJECT_SOURCE_DIR}/libtitanium/resource/readbuf_source_file.cpp

    ${PROJECT_SOURCE_DIR}/libtitanium/resource/filetype/targa.cpp
)

#add_library( libtitanium_gui STATIC )
#target_include_directories( libtitanium_sys PUBLIC ${PROJECT_SOURCE_DIR} )
#target_sources( libtitanium_gui PRIVATE
#     ${PROJECT_SOURCE_DIR}/libtitanium/gui/gui_layout.cpp
#)
#target_link_libraries( libtitanium_gui PRIVATE libtitaniun_renderer libtitanium_input )
#target_compile_options( libtitanium_gui INTERFACE ${TITANIUM_COMPILER_ARGS} )
