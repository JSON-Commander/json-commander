# Commander

A C++20 library for defining command line interfaces via JSON schemas.

Inspired by OCaml's [cmdliner](https://erratique.ch/software/cmdliner),
Commander takes a declarative approach: describe your CLI as a JSON schema,
and Commander handles argument parsing, environment variable fallback,
configuration file merging, man page generation, and runtime configuration
output.

## Features

- **Declarative CLI specification** -- define arguments, options, flags,
  subcommands, and their documentation in JSON
- **Argument parsing** -- long/short options, flag grouping (`-abc`),
  positional arguments, `--` termination, nested subcommands
- **Environment variable fallback** -- options and flags can fall back to
  environment variables when not provided on the command line
- **Man page generation** -- produce groff output suitable for `man(1)` or
  plain-text help for `--help`
- **Config schema generation** -- emit a JSON Schema (draft 2020-12)
  describing the runtime configuration output
- **Schema validation** -- validate CLI schemas against the commander
  metaschema before use
- **Unified JSON output** -- parsing produces a flat JSON object ready for
  application consumption

## Quick Start

Define your CLI as a `model::Root`, compile it, parse arguments, and use the
resulting JSON configuration:

```cpp
#include <commander/cmd.hpp>
#include <commander/manpage.hpp>
#include <commander/parse.hpp>

#include <iostream>

using namespace commander;

model::Root make_cli() {
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

int main(int argc, char *argv[]) {
  auto cli = make_cli();
  auto spec = cmd::make(cli);
  auto result = parse::parse(spec, {argv + 1, argv + argc});

  if (auto *ok = std::get_if<parse::ParseOk>(&result)) {
    std::cout << "Hello, " << ok->config["name"].get<std::string>() << "!\n";
    return 0;
  }
  if (auto *help = std::get_if<parse::HelpRequest>(&result)) {
    std::cout << manpage::to_plain_text(cli, help->command_path);
    return 0;
  }
  if (std::holds_alternative<parse::VersionRequest>(result)) {
    std::cout << "greet " << *cli.version << "\n";
    return 0;
  }
  return 1;
}
```

CLIs can also be defined as JSON files and loaded at runtime via
`schema::Loader`:

```json
{
  "name": "greet",
  "doc": ["A friendly greeting tool."],
  "version": "1.0.0",
  "args": [
    {
      "kind": "flag",
      "names": ["loud", "l"],
      "doc": ["Print the greeting in uppercase."]
    },
    {
      "kind": "positional",
      "name": "name",
      "doc": ["The name to greet."],
      "type": "string",
      "required": true
    }
  ]
}
```

## Commander CLI Tool

Commander ships with a `commander` tool that dogfoods the library:

```sh
commander validate schema.json          # Validate a schema
commander help schema.json              # Print plain-text help
commander man schema.json               # Generate groff man page
commander man schema.json commit        # Man page for a subcommand
commander config-schema schema.json     # Generate runtime config JSON Schema
commander parse schema.json -- --loud Alice  # Parse args, output JSON
```

## Building

Commander uses CMake with Ninja Multi-Config. Dependencies are fetched
automatically via FetchContent.

```sh
cmake --preset <preset-name>
cmake --build build-<preset-name> --config Release
ctest --test-dir build-<preset-name> -C Release --output-on-failure
```

Presets are defined per-compiler in `CMakeUserPresets.json` (auto-generated
from `compilers.json`). The base hidden preset lives in `CMakePresets.json`.

### Compiler Requirements

C++20. Tested with Clang 11--20 and GCC 10--12.

### Build Configurations

| Configuration | Flags |
|---|---|
| Release | `-g -DNDEBUG -O3` |
| RelWithDebInfo | `-fsanitize=undefined -fsanitize=address -g -O3` |

All builds use `-Wall -Wextra -pedantic -Werror`.

## Dependencies

Managed via FetchContent through the `cmake_utilities` submodule:

| Dependency | Purpose |
|---|---|
| [nlohmann/json](https://github.com/nlohmann/json) v3.10.0 | JSON parsing and representation |
| [nlohmann/json-schema-validator](https://github.com/pboettch/json-schema-validator) | JSON Schema validation |
| [Catch2](https://github.com/catchorg/Catch2) | Testing framework |

## Project Structure

```
commander/                 Library headers
  schema/                  Commander metaschema (JSON Schema)
  model.hpp                C++ data model (Root, Command, Argument, ...)
  model_json.hpp           JSON serialization/deserialization
  schema_loader.hpp        Schema validation and loading
  conv.hpp                 String-to-JSON type converters
  validate.hpp             Constraint validators (required, must_exist, ...)
  arg.hpp                  Compiled argument specifications
  cmd.hpp                  Command/subcommand compilation
  parse.hpp                Argument parsing engine
  manpage.hpp              Man page and help text generation
  config_schema.hpp        Runtime config JSON Schema generation
commander_testing/         Test sources (Catch2)
examples/
  greet/                   Simple flag + positional example
  serve/                   Schema-driven server example
  fake-git/                Nested subcommands example (modeled after git)
tools/
  commander.cpp            Unified CLI tool
  commander.json           CLI tool's own schema (self-hosting)
```

## Architecture

Commander follows a pipeline design:

```
JSON Schema  -->  model::Root  -->  cmd::RootSpec  -->  parse::parse()  -->  JSON config
                       |
                       +-->  manpage::to_groff() / to_plain_text()
                       +-->  config_schema::to_config_schema()
```

1. **Model** (`model.hpp`) -- C++ types mirroring the JSON schema: `Root`,
   `Command`, `Argument` (variant of `Flag`, `Option`, `Positional`,
   `FlagGroup`), `TypeSpec`, and documentation types.

2. **Schema Loader** (`schema_loader.hpp`) -- validates a JSON document
   against the commander metaschema and deserializes it into `model::Root`.

3. **Converters** (`conv.hpp`) -- bidirectional string-to-JSON converters
   for scalar types (string, int, float, bool, enum, file, dir, path) and
   compound types (list, pair, triple).

4. **Validators** (`validate.hpp`) -- constraint checkers (required,
   must_exist) composed via `all_of`.

5. **Arg/Cmd** (`arg.hpp`, `cmd.hpp`) -- compile model types into
   parsing-ready specifications with resolved defaults, bundled converters,
   and validators.

6. **Parser** (`parse.hpp`) -- consumes compiled specs and CLI tokens,
   produces `ParseResult` (variant of `ParseOk`, `HelpRequest`,
   `ManpageRequest`, `VersionRequest`).

7. **Man page** (`manpage.hpp`) -- assembles man page sections from model
   types, renders to groff or plain text.

8. **Config schema** (`config_schema.hpp`) -- generates a JSON Schema
   describing the runtime configuration that `parse::parse` produces.

## License

See [LICENSE](LICENSE) for details.
