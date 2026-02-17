// serve — A schema-driven CLI example using JSON-Commander.
//
// Demonstrates:
//   - Loading a CLI definition from a JSON schema file via schema::Loader
//   - Typed options (int, string) with default values
//   - Environment variable bindings (SERVE_PORT, SERVE_HOST, SERVE_VERBOSE)
//   - Catching schema::Error alongside parse::Error

#include <json_commander/cmd.hpp>
#include <json_commander/manpage.hpp>
#include <json_commander/parse.hpp>
#include <json_commander/schema_loader.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace json_commander;

// ---------------------------------------------------------------------------
// CLI definition
// ---------------------------------------------------------------------------

model::Root
make_cli() {
  schema::Loader loader;
  return loader.load(std::string(SERVE_SCHEMA));
}

// ---------------------------------------------------------------------------
// Application logic
// ---------------------------------------------------------------------------

int
run(const std::vector<std::string> &args) {
  auto cli = make_cli();
  auto spec = cmd::make(cli);
  auto result = parse::parse(spec, args);

  if (auto *ok = std::get_if<parse::ParseOk>(&result)) {
    auto port = ok->config["port"].get<int>();
    auto host = ok->config["host"].get<std::string>();
    auto dir = ok->config["dir"].get<std::string>();
    auto verbose = ok->config["verbose"].get<bool>();

    std::cout << "Serving " << dir << " on " << host << ":" << port;
    if (verbose) {
      std::cout << " (verbose)";
    }
    std::cout << "\n";
    return 0;
  }

  if (auto *help = std::get_if<parse::HelpRequest>(&result)) {
    std::cout << manpage::to_plain_text(cli, help->command_path);
    return 0;
  }

  if (auto *man = std::get_if<parse::ManpageRequest>(&result)) {
    std::cout << manpage::to_groff(cli, man->command_path);
    return 0;
  }

  if (std::holds_alternative<parse::VersionRequest>(result)) {
    std::cout << "serve " << *cli.version << "\n";
    return 0;
  }

  return 1;
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int
main(int argc, char *argv[]) {
  try {
    return run({argv + 1, argv + argc});
  } catch (const schema::Error &e) {
    std::cerr << "schema error: " << e.what() << "\n";
    return 1;
  } catch (const parse::Error &e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
}
