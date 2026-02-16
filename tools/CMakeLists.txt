add_executable(commander commander.cpp)
target_link_libraries(commander PRIVATE
  commander::header
  commander::library
  nlohmann_json::nlohmann_json
  nlohmann_json_schema_validator)
set_target_properties(commander PROPERTIES
  CXX_STANDARD ${commander_CXX_STANDARD})
target_compile_definitions(commander PRIVATE
  COMMANDER_TOOL_SCHEMA="${CMAKE_CURRENT_SOURCE_DIR}/commander.json")
