cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(_project_name googletest)

#
# Parameters repository
#
set(${_project_name}_repo "https://github.com/google/googletest.git")
set(${_project_name}_tag "release-1.8.0")

set(${_project_name}_source_dir   ${_project_name}_src)
set(${_project_name}_build_dir    ${_project_name}_build)
set(${_project_name}_download_dir ${_project_name}_download)

# Configure package
configure_file(CMakeLists.txt.in
               ${${_project_name}_download_dir}/CMakeLists.txt)
               
#
# Download package
#
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${${_project_name}_download_dir} )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${${_project_name}_download_dir} )

 # Google Test also has to use one.

set( gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
  
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/${${_project_name}_source_dir}
                 ${CMAKE_CURRENT_BINARY_DIR}/${${_project_name}_build_dir})