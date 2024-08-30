# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\VideoexcesV3fixed_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\VideoexcesV3fixed_autogen.dir\\ParseCache.txt"
  "VideoexcesV3fixed_autogen"
  )
endif()
