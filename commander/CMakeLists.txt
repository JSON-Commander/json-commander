configure_file(config.hpp.in config.hpp)

add_library(commander_header INTERFACE)
set_target_properties(commander_header PROPERTIES EXPORT_NAME header)
target_include_directories(commander_header
  INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${commander_INSTALL_INCLUDEDIR}>)

add_library(commander::header ALIAS commander_header)
  
install(TARGETS commander_header
  EXPORT commander_EXPORTS
  RUNTIME DESTINATION ${commander_INSTALL_BINDIR}
  LIBRARY DESTINATION ${commander_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${commander_INSTALL_LIBDIR})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
  DESTINATION ${commander_INSTALL_INCLUDEDIR}/commander)
