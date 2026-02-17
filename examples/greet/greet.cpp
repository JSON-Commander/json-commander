// greet — A simple CLI example using JSON-Commander.
//
// Demonstrates:
//   - Building a model::Root with a flag and a positional
//   - Compiling to cmd::RootSpec via cmd::make
//   - Parsing argc/argv via parse::parse
//   - Handling all three ParseResult variants

#include <json_commander/cmd.hpp>
#include <json_commander/manpage.hpp>
#include <json_commander/parse.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

using namespace json_commander;

// ---------------------------------------------------------------------------
// CLI definition
// ---------------------------------------------------------------------------

model::Root
make_cli() {
  model::Flag loud;
  loud.names = {"loud", "l"};
  loud.doc = {"Print the greeting in uppercase."};

  model::Positional name;
  name.name = "name";
  name.doc = {"The name to greet."};
  name.type = model::ScalarType::String;
  name.required = true;

  model::Root root;
  root.name = "greet";
  root.doc = {"A friendly greeting tool."};
  root.version = "1.0.0";
  root.args = std::vector<model::Argument>{loud, name};
  return root;
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
    std::string greeting = "Hello, " + ok->config["name"].get<std::string>() + "!";
    if (ok->config["loud"].get<bool>()) {
      std::transform(greeting.begin(), greeting.end(), greeting.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
      });
    }
    std::cout << greeting << "\n";
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
    std::cout << "greet " << *cli.version << "\n";
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
  } catch (const parse::Error &e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
}
