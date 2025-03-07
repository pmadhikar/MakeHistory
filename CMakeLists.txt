cmake_minimum_required(VERSION 3.20)
include(FetchContent)

project(MakeHistory)



FetchContent_Declare(
  libgit2
  GIT_REPOSITORY https://github.com/libgit2/libgit2.git
  GIT_TAG 338e6fb681369ff0537719095e22ce9dc602dbf0
)

set(BUILD_CLAR OFF)
set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(libgit2)
unset(BUILD_CLAR)
unset(BUILD_SHARED_LIBS)

file(GLOB make_history_src
  CONFIGURE_DEPENDS
  "src/*.cpp"
  "images/*.hpp"
  "src/*.hpp"
)

add_executable(MakeHistory ${make_history_src})

target_include_directories(MakeHistory PRIVATE .)

target_link_libraries(MakeHistory libgit2 util libgit2package)

target_compile_features(MakeHistory PUBLIC cxx_std_23)
