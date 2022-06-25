# CMakeList.txt : CMake project for GetDiscogsVideos, include source and define
# project specific logic here.

cmake_minimum_required (VERSION 3.22)

# Add source to this project's executable.
add_executable (GetDiscogsVideos "GetDiscogsVideos.cpp" "GetDiscogsVideos.h" "Constants.h" "Functions.h" "Functions.cpp"       "Progress.cpp" "Progress.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GetDiscogsVideos PROPERTY CXX_STANDARD 20)
endif()
