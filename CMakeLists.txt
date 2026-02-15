cmake_minimum_required(VERSION 3.12)

project(commander VERSION 0.1.0 LANGUAGES C CXX)

option(commander_BUILD_TESTING "Build the commander tests" ON)
option(commander_BUILD_EXAMPLES "Build the commander examples" ON)
set(commander_CXX_STANDARD 20 CACHE STRING "C++ standard for commander")

include(GNUInstallDirs)
set(commander_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
set(commander_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(commander_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
set(commander_INSTALL_CONFDIR ${commander_INSTALL_LIBDIR}/cmake/commander-${PROJECT_VERSION})

list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)
include(commander_deps)

add_subdirectory(commander)

if(commander_BUILD_TESTING)
  include(CTest)
  if(BUILD_TESTING)
    add_subdirectory(commander_testing)
  endif()
endif()

if(commander_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

install(EXPORT commander_EXPORTS
  NAMESPACE commander::
  FILE commander-exports.cmake
  DESTINATION ${commander_INSTALL_CONFDIR})

configure_file(commander-config.cmake.in commander-config.cmake)
install(FILES ${PROJECT_BINARY_DIR}/commander-config.cmake
  DESTINATION ${commander_INSTALL_CONFDIR})
