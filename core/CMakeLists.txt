set(_component_name core)

set(${_project_name}_${_component_name}_library_type
    ""
    CACHE STRING "Type of ${_project_name}_${_component_name} library to be created (STATIC|SHARED).")

set(${_project_name}_${_component_name}_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}
    CACHE INTERNAL "${_project_name}_${_component_name} headers directory.")

set(${_project_name}_${_component_name}_HEADERS 
    include/threadpoolex/core/export.hpp
    include/threadpoolex/core/ITask.hpp
    include/threadpoolex/core/TNotifier.hpp
    include/threadpoolex/core/TLocking.hpp
    include/threadpoolex/core/IThreadPool.hpp
    include/threadpoolex/core/RAII.hpp
    CACHE INTERNAL "${_project_name}_${_component_name} headers.")

set(${_project_name}_${_component_name}_VERSION
    ${CMAKE_BINARY_DIR}/version/version.hpp
    CACHE INTERNAL "${_project_name}_${_component_name} versions.")
    
set(${_project_name}_${_component_name}_SOURCES
    src/ThreadPool.cpp
    CACHE INTERNAL "${_project_name}_${_component_name} sources")

add_library(
    ${_project_name}_${_component_name}
    ${${_project_name}_${_component_name}_library_type}
    ${${_project_name}_${_component_name}_HEADERS}
    ${${_project_name}_${_component_name}_SOURCES}
    ${${_project_name}_${_component_name}_VERSION}
    )

#
# Add include directories to a target.
#
target_include_directories(
    ${_project_name}_${_component_name}
    PUBLIC ${${_project_name}_${_component_name}_INCLUDE_DIR}
    )

#
# Link a target to given libraries.
#
target_link_libraries(
    ${_project_name}_${_component_name}
    )

#
# Put a list of test sources into the cache.
#
set(${_project_name}_${_component_name}_TESTS
   test/TLocking_test.cpp
    CACHE INTERNAL "${_project_name}_${_component_name} tests")

#
# Add test module if tests sources are specified and skip this step overwise.
#
if(${_project_name}_${_component_name}_TESTS)

    #
    # Add an test executable to the project using the specified source files.
    #
    add_executable(
        ${_project_name}_${_component_name}_test
        ${${_project_name}_${_component_name}_TESTS})

    #
    # Link a test target to given libraries.
    #
    target_link_libraries(
        ${_project_name}_${_component_name}_test
        ${CMAKE_THREAD_LIBS_INIT}
        ${_project_name}_${_component_name}
        gtest
        gtest_main)

else()
    message(STATUS "Skipping test module generation for ${_project_name}_${_component_name}")
endif()


function(layout_source_groups)
    set(_options)
    set(_oneValueArgs)
    set(_multiValueArgs ITEMS)
    cmake_parse_arguments(_args "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN})

    # Process items
    foreach(_item ${_args_ITEMS})
        foreach(_file ${_item}) 
            get_filename_component(_group "${_file}" PATH)
            string(REPLACE "/" "\\" _group "${_group}")
            source_group("${_group}" FILES "${_file}")
        endforeach()
    endforeach()

endfunction()

layout_source_groups(ITEMS
    ${${_project_name}_${_component_name}_HEADERS}
    ${${_project_name}_${_component_name}_SOURCES}
    ${${_project_name}_${_component_name}_TESTS})