# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\studentManagementSystem_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\studentManagementSystem_autogen.dir\\ParseCache.txt"
  "studentManagementSystem_autogen"
  )
endif()
