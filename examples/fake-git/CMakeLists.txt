commander_add_example(fake-git)
target_compile_definitions(fake-git PRIVATE
  FAKEGIT_SCHEMA="${CMAKE_CURRENT_SOURCE_DIR}/fake-git.json")
