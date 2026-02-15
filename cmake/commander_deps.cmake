if(NOT EXISTS ${PROJECT_SOURCE_DIR}/cmake_utilities/FindCMakeUtilities.cmake)
  find_package(Git REQUIRED)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_utilities)
find_package(CMakeUtilities)

find_package(nlohmann_json REQUIRED)
install(TARGETS nlohmann_json
  EXPORT HACK_TO_SATISFY_CMAKE_FOR_VALIDATOR
)
install(EXPORT HACK_TO_SATISFY_CMAKE_FOR_VALIDATOR
  NAMESPACE barf::
  FILE hack-for-validator.cmake
  DESTINATION ${commander_INSTALL_CONFDIR})

find_package(nlohmann_json_schema_validator REQUIRED)
