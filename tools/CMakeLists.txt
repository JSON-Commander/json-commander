add_executable(json-commander json_commander.cpp)
target_link_libraries(json-commander PRIVATE
  json_commander::header
  json_commander::library
  nlohmann_json::nlohmann_json
  nlohmann_json_schema_validator)
set_target_properties(json-commander PROPERTIES
  CXX_STANDARD ${json_commander_CXX_STANDARD})
target_compile_definitions(json-commander PRIVATE
  JSON_COMMANDER_TOOL_SCHEMA="${CMAKE_CURRENT_SOURCE_DIR}/json_commander.json")
