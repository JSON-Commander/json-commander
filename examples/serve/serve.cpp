// serve — A schema-driven CLI example using JSON-Commander.
//
// Demonstrates:
//   - Using json_commander::run_file() to load a CLI from a JSON schema
//   - Typed options (int, string) with default values
//   - Environment variable bindings (SERVE_PORT, SERVE_HOST, SERVE_VERBOSE)

#include <json_commander/run.hpp>

#include <iostream>
#include <string>

int
main(int argc, char *argv[]) {
  return json_commander::run_file(SERVE_SCHEMA, argc, argv, [](const nlohmann::json &config) {
    auto port = config["port"].get<int>();
    auto host = config["host"].get<std::string>();
    auto dir = config["dir"].get<std::string>();
    auto verbose = config["verbose"].get<bool>();

    std::cout << "Serving " << dir << " on " << host << ":" << port;
    if (verbose) {
      std::cout << " (verbose)";
    }
    std::cout << "\n";
    return 0;
  });
}
