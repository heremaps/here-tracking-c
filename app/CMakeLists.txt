
include_directories(include)

add_subdirectory(src)

if(BuildTests)
  add_subdirectory(test)
endif()
