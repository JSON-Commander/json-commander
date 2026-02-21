macro(json_commander_add_example example_name)
  add_executable(${example_name} ${example_name}.cpp)
  target_link_libraries(${example_name} PRIVATE
    json_commander::header
    json_commander::library
    nlohmann_json::nlohmann_json
    nlohmann_json_schema_validator)
  set_target_properties(${example_name}
    PROPERTIES
    CXX_STANDARD ${json_commander_CXX_STANDARD})
endmacro()

macro(json_commander_add_c_example example_name)
  add_executable(${example_name} ${example_name}.c)
  target_link_libraries(${example_name} PRIVATE json_commander::c)
  set_target_properties(${example_name}
    PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    LINKER_LANGUAGE CXX)
endmacro()

add_subdirectory(fake-git)
add_subdirectory(greet)
add_subdirectory(serve)

if(json_commander_BUILD_C_API)
  add_subdirectory(greet-c)
  add_subdirectory(serve-c)
endif()
