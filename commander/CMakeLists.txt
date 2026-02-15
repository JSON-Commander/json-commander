configure_file(config.hpp.in config.hpp)

add_library(commander_header INTERFACE)
set_target_properties(commander_header PROPERTIES EXPORT_NAME header)
target_include_directories(commander_header
  INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${commander_INSTALL_INCLUDEDIR}>)

add_library(commander::header ALIAS commander_header)

add_library(commander_library INTERFACE)
set_target_properties(commander_library PROPERTIES EXPORT_NAME library)
target_compile_definitions(commander_library
  INTERFACE
  COMMANDER_SCHEMA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/schema")
add_library(commander::library ALIAS commander_library)

install(TARGETS commander_header commander_library
  EXPORT commander_EXPORTS
  RUNTIME DESTINATION ${commander_INSTALL_BINDIR}
  LIBRARY DESTINATION ${commander_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${commander_INSTALL_LIBDIR})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
  DESTINATION ${commander_INSTALL_INCLUDEDIR}/commander)

install(DIRECTORY schema/
  DESTINATION ${CMAKE_INSTALL_DATADIR}/commander/schema
  FILES_MATCHING PATTERN "*.json")
