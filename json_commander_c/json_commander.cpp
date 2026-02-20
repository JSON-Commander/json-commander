#include "json_commander.h"

#include <json_commander/cmd.hpp>
#include <json_commander/manpage.hpp>
#include <json_commander/parse.hpp>
#include <json_commander/schema_loader.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace json_commander;

static std::string
extract_name(int argc, char *argv[]) {
  if (argc > 0 && argv && argv[0] && argv[0][0] != '\0') {
    return argv[0];
  }
  return "error";
}

int
jcmd_run(const char *cli_json, int argc, char *argv[], jcmd_main_fn main_fn) {
  auto name = extract_name(argc, argv);

  model::Root root;
  try {
    schema::Loader loader;
    auto j = nlohmann::json::parse(cli_json);
    root = loader.load(j);
  } catch (...) {
    std::cerr << name
              << ": invalid CLI definition. Use json-commander validate to check your schema.\n";
    return 1;
  }

  auto spec = cmd::make(root);

  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  parse::ParseResult result;
  try {
    result = parse::parse(spec, args);
  } catch (const parse::Error &e) {
    std::cerr << name << ": " << e.what() << "\n";
    std::cerr << manpage::to_plain_text(root, {});
    return 1;
  }

  return std::visit(
      [&](const auto &r) -> int {
        using T = std::decay_t<decltype(r)>;

        if constexpr (std::is_same_v<T, parse::ParseOk>) {
          return main_fn(r.config.dump().c_str());
        } else if constexpr (std::is_same_v<T, parse::HelpRequest>) {
          std::cout << manpage::to_plain_text(root, r.command_path);
          return 0;
        } else if constexpr (std::is_same_v<T, parse::VersionRequest>) {
          std::cout << name << " version";
          if (root.version) {
            std::cout << " " << *root.version;
          }
          std::cout << "\n";
          return 0;
        } else if constexpr (std::is_same_v<T, parse::ManpageRequest>) {
          std::cout << manpage::to_groff(root, r.command_path);
          return 0;
        }
      },
      result);
}
