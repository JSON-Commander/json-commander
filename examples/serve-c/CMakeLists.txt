json_commander_add_c_example(serve-c)
target_compile_definitions(serve-c PRIVATE
  SERVE_C_SCHEMA="${CMAKE_CURRENT_SOURCE_DIR}/../serve/serve.json")
