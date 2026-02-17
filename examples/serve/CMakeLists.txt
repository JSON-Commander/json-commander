json_commander_add_example(serve)
target_compile_definitions(serve PRIVATE
  SERVE_SCHEMA="${CMAKE_CURRENT_SOURCE_DIR}/serve.json")
