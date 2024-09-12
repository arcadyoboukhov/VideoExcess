# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\VideoExcess_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\VideoExcess_autogen.dir\\ParseCache.txt"
  "VideoExcess_autogen"
  )
endif()
