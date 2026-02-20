add_library(json_commander_c SHARED json_commander.cpp)
set_target_properties(json_commander_c PROPERTIES
  OUTPUT_NAME json_commander
  EXPORT_NAME c
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  CXX_STANDARD ${json_commander_CXX_STANDARD}
  POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(json_commander_c PRIVATE JCMD_BUILDING)
target_include_directories(json_commander_c
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${json_commander_INSTALL_INCLUDEDIR}>)
target_link_libraries(json_commander_c PRIVATE
  json_commander::header
  json_commander::library
  nlohmann_json::nlohmann_json
  nlohmann_json_schema_validator)

add_library(json_commander::c ALIAS json_commander_c)

install(TARGETS json_commander_c
  EXPORT json_commander_EXPORTS
  RUNTIME DESTINATION ${json_commander_INSTALL_BINDIR}
  LIBRARY DESTINATION ${json_commander_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${json_commander_INSTALL_LIBDIR})

install(FILES json_commander.h
  DESTINATION ${json_commander_INSTALL_INCLUDEDIR})
