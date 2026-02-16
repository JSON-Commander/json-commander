add_executable(commander-config-schema config_schema_tool.cpp)
target_link_libraries(commander-config-schema PRIVATE
  commander::header
  commander::library
  nlohmann_json::nlohmann_json
  nlohmann_json_schema_validator)
set_target_properties(commander-config-schema
  PROPERTIES
  CXX_STANDARD ${commander_CXX_STANDARD})
