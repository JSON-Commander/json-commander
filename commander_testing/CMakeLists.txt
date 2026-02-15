find_package(Catch2 REQUIRED)

macro(commander_add_test test_name)
  set(exe_name ${test_name}_test)
  set(src_name ${exe_name}.cpp)
  add_executable(${exe_name} ${src_name})
  target_link_libraries(${exe_name} PRIVATE
    commander::header
    commander::library
    nlohmann_json::nlohmann_json
    nlohmann_json_schema_validator
    Catch2::Catch2WithMain)
  set_target_properties(${exe_name}
    PROPERTIES
    CXX_STANDARD ${commander_CXX_STANDARD})
  add_test(${test_name} ${exe_name})
endmacro()

commander_add_test(metaschema)
commander_add_test(model)
commander_add_test(schema_loader)
commander_add_test(conv)
commander_add_test(validate)
commander_add_test(arg)
commander_add_test(cmd)
commander_add_test(manpage)
commander_add_test(parse)
