// commander-config-schema — Generate a JSON Schema for runtime configuration.
//
// Given a commander schema file, outputs the JSON Schema describing
// the runtime configuration that commander would produce.
//
// Usage: commander-config-schema <schema-file> [subcommand...]

#include <commander/config_schema.hpp>
#include <commander/schema_loader.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace commander;

// ---------------------------------------------------------------------------
// Usage
// ---------------------------------------------------------------------------

void
print_usage(const char *program) {
  std::cerr << "Usage: " << program << " <schema-file> [subcommand...]\n";
}

// ---------------------------------------------------------------------------
// Core logic
// ---------------------------------------------------------------------------

int
run(const char *program, const std::vector<std::string> &args) {
  if (args.empty()) {
    print_usage(program);
    return 1;
  }

  const auto &schema_path = args[0];
  std::vector<std::string> command_path(args.begin() + 1, args.end());

  schema::Loader loader;
  auto root = loader.load(schema_path);
  auto schema = config_schema::to_config_schema(root, command_path);

  std::cout << schema.dump(2) << "\n";
  return 0;
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int
main(int argc, char *argv[]) {
  try {
    return run(argv[0], {argv + 1, argv + argc});
  } catch (const schema::Error &e) {
    std::cerr << "schema error: " << e.what() << "\n";
    return 1;
  } catch (const std::runtime_error &e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
}
