#include <catch2/catch_test_macros.hpp>
#include <commander/manpage.hpp>

using namespace commander;
using namespace commander::manpage;

// ---------------------------------------------------------------------------
// Phase 1: Groff escape and DocString rendering
// ---------------------------------------------------------------------------

TEST_CASE("groff::escape passes through plain text unchanged", "[manpage]") {
  REQUIRE(groff::escape("hello world") == "hello world");
}

TEST_CASE("groff::escape escapes backslash", "[manpage]") {
  REQUIRE(groff::escape("a\\b") == "a\\\\b");
}

TEST_CASE("groff::escape escapes leading period", "[manpage]") {
  REQUIRE(groff::escape(".TH") == "\\&.TH");
}

TEST_CASE("groff::escape escapes leading apostrophe", "[manpage]") {
  REQUIRE(groff::escape("'hello") == "\\&'hello");
}

TEST_CASE("groff::escape handles empty string", "[manpage]") {
  REQUIRE(groff::escape("") == "");
}

TEST_CASE("docstring_to_text joins lines with spaces", "[manpage]") {
  model::DocString doc = {"first line", "second line", "third line"};
  REQUIRE(detail::docstring_to_text(doc) == "first line second line third line");
}

TEST_CASE("docstring_to_text treats empty element as paragraph break", "[manpage]") {
  model::DocString doc = {"first paragraph", "", "second paragraph"};
  REQUIRE(detail::docstring_to_text(doc) == "first paragraph\n\nsecond paragraph");
}

TEST_CASE("docstring_to_text on single element", "[manpage]") {
  model::DocString doc = {"only line"};
  REQUIRE(detail::docstring_to_text(doc) == "only line");
}

TEST_CASE("docstring_to_text on empty doc", "[manpage]") {
  model::DocString doc = {};
  REQUIRE(detail::docstring_to_text(doc) == "");
}

// ---------------------------------------------------------------------------
// Phase 2: Groff block rendering
// ---------------------------------------------------------------------------

TEST_CASE("render_block ParagraphBlock produces .PP and escaped text", "[manpage]") {
  model::ParagraphBlock block{{"Hello world."}};
  REQUIRE(groff::render_block(block) == ".PP\nHello world.\n");
}

TEST_CASE("render_block ParagraphBlock with multi-line doc", "[manpage]") {
  model::ParagraphBlock block{{"first line", "second line"}};
  REQUIRE(groff::render_block(block) == ".PP\nfirst line second line\n");
}

TEST_CASE("render_block PreBlock produces .nf/.fi with lines", "[manpage]") {
  model::PreBlock block{{"line one", "line two"}};
  REQUIRE(groff::render_block(block) == ".nf\nline one\nline two\n.fi\n");
}

TEST_CASE("render_block LabelTextBlock produces .TP with bold label", "[manpage]") {
  model::LabelTextBlock block{"--verbose", {"Enable verbose output."}};
  REQUIRE(groff::render_block(block) == ".TP\n\\fB--verbose\\fR\nEnable verbose output.\n");
}

TEST_CASE("render_block NoBlankBlock produces empty string", "[manpage]") {
  model::NoBlankBlock block{};
  REQUIRE(groff::render_block(block) == "");
}

TEST_CASE("render_section produces .SH header and rendered blocks", "[manpage]") {
  model::ManSection section{
      "NAME",
      {model::ParagraphBlock{{"mytool \\- a test tool"}}},
  };
  REQUIRE(groff::render_section(section) == ".SH NAME\n.PP\nmytool \\\\- a test tool\n");
}

// ---------------------------------------------------------------------------
// Phase 3: Argument label formatting
// ---------------------------------------------------------------------------

TEST_CASE("format_names single long name", "[manpage]") {
  model::ArgNames names = {"--verbose"};
  REQUIRE(detail::format_names(names) == "\\fB\\-\\-verbose\\fR");
}

TEST_CASE("format_names single short name", "[manpage]") {
  model::ArgNames names = {"-v"};
  REQUIRE(detail::format_names(names) == "\\fB\\-v\\fR");
}

TEST_CASE("format_names short and long", "[manpage]") {
  model::ArgNames names = {"-v", "--verbose"};
  REQUIRE(detail::format_names(names) == "\\fB\\-v\\fR, \\fB\\-\\-verbose\\fR");
}

TEST_CASE("format_option_label with explicit docv", "[manpage]") {
  model::Option opt{};
  opt.names = {"--count"};
  opt.doc = {"A count."};
  opt.type = model::ScalarType::Int;
  opt.docv = "COUNT";
  REQUIRE(detail::format_option_label(opt) == "\\fB\\-\\-count\\fR=\\fICOUNT\\fR");
}

TEST_CASE("format_option_label without docv uses converter docv", "[manpage]") {
  model::Option opt{};
  opt.names = {"--count"};
  opt.doc = {"A count."};
  opt.type = model::ScalarType::Int;
  REQUIRE(detail::format_option_label(opt) == "\\fB\\-\\-count\\fR=\\fIINT\\fR");
}

TEST_CASE("format_option_label short and long with docv", "[manpage]") {
  model::Option opt{};
  opt.names = {"-c", "--count"};
  opt.doc = {"A count."};
  opt.type = model::ScalarType::Int;
  opt.docv = "COUNT";
  REQUIRE(detail::format_option_label(opt) ==
          "\\fB\\-c\\fR \\fICOUNT\\fR, \\fB\\-\\-count\\fR=\\fICOUNT\\fR");
}

TEST_CASE("format_positional_label uses docv or name", "[manpage]") {
  model::Positional pos{};
  pos.name = "file";
  pos.doc = {"A file argument."};
  pos.type = model::ScalarType::File;
  REQUIRE(detail::format_positional_label(pos) == "\\fIFILE\\fR");
}

TEST_CASE("format_positional_label with custom docv", "[manpage]") {
  model::Positional pos{};
  pos.name = "file";
  pos.doc = {"A file argument."};
  pos.type = model::ScalarType::File;
  pos.docv = "INPUT";
  REQUIRE(detail::format_positional_label(pos) == "\\fIINPUT\\fR");
}

TEST_CASE("format_flag_group_entry_label formats like flag names", "[manpage]") {
  model::FlagGroupEntry entry{
      {"-q", "--quiet"},
      {"Be quiet."},
      nlohmann::json(true),
  };
  REQUIRE(detail::format_flag_group_entry_label(entry) ==
          "\\fB\\-q\\fR, \\fB\\-\\-quiet\\fR");
}

// ---------------------------------------------------------------------------
// Phase 4: Argument documentation blocks
// ---------------------------------------------------------------------------

TEST_CASE("make_arg_sections for single flag produces OPTIONS section", "[manpage]") {
  model::Flag flag{};
  flag.names = {"-v", "--verbose"};
  flag.doc = {"Enable verbose output."};

  std::vector<model::Argument> args = {flag};
  auto sections = make_arg_sections(args);

  REQUIRE(sections.size() == 1);
  REQUIRE(sections[0].name == "OPTIONS");
  REQUIRE(sections[0].blocks.size() == 1);
}

TEST_CASE("make_arg_sections for single option produces OPTIONS section", "[manpage]") {
  model::Option opt{};
  opt.names = {"--count"};
  opt.doc = {"A count."};
  opt.type = model::ScalarType::Int;
  opt.docv = "COUNT";

  std::vector<model::Argument> args = {opt};
  auto sections = make_arg_sections(args);

  REQUIRE(sections.size() == 1);
  REQUIRE(sections[0].name == "OPTIONS");
  REQUIRE(sections[0].blocks.size() == 1);
}

TEST_CASE("make_arg_sections for positional produces ARGUMENTS section", "[manpage]") {
  model::Positional pos{};
  pos.name = "file";
  pos.doc = {"A file."};
  pos.type = model::ScalarType::File;

  std::vector<model::Argument> args = {pos};
  auto sections = make_arg_sections(args);

  REQUIRE(sections.size() == 1);
  REQUIRE(sections[0].name == "ARGUMENTS");
}

TEST_CASE("make_arg_sections for flag group produces one entry per flag", "[manpage]") {
  model::FlagGroup group{};
  group.dest = "level";
  group.doc = {"Set level."};
  group.default_value = "normal";
  group.flags = {
      model::FlagGroupEntry{{"-q", "--quiet"}, {"Be quiet."}, nlohmann::json("quiet")},
      model::FlagGroupEntry{{"--loud"}, {"Be loud."}, nlohmann::json("loud")},
  };

  std::vector<model::Argument> args = {group};
  auto sections = make_arg_sections(args);

  REQUIRE(sections.size() == 1);
  REQUIRE(sections[0].name == "OPTIONS");
  REQUIRE(sections[0].blocks.size() == 2);
}

TEST_CASE("make_arg_sections groups by docs field", "[manpage]") {
  model::Flag flag{};
  flag.names = {"--verbose"};
  flag.doc = {"Verbose."};

  model::Positional pos{};
  pos.name = "file";
  pos.doc = {"A file."};
  pos.type = model::ScalarType::File;

  std::vector<model::Argument> args = {flag, pos};
  auto sections = make_arg_sections(args);

  REQUIRE(sections.size() == 2);
  // Sort order: ARGUMENTS before OPTIONS based on standard ordering
  bool has_options = false;
  bool has_arguments = false;
  for (const auto &s : sections) {
    if (s.name == "OPTIONS")
      has_options = true;
    if (s.name == "ARGUMENTS")
      has_arguments = true;
  }
  REQUIRE(has_options);
  REQUIRE(has_arguments);
}

TEST_CASE("make_arg_sections with custom docs field", "[manpage]") {
  model::Flag flag{};
  flag.names = {"--debug"};
  flag.doc = {"Debug mode."};
  flag.docs = "DEBUGGING";

  std::vector<model::Argument> args = {flag};
  auto sections = make_arg_sections(args);

  REQUIRE(sections.size() == 1);
  REQUIRE(sections[0].name == "DEBUGGING");
}

// ---------------------------------------------------------------------------
// Phase 5: Auto-generated sections (NAME, SYNOPSIS, COMMANDS)
// ---------------------------------------------------------------------------

TEST_CASE("make_name_section produces NAME with name and doc", "[manpage]") {
  auto section = make_name_section("mytool", {"A cool tool."});
  REQUIRE(section.name == "NAME");
  REQUIRE(section.blocks.size() == 1);
  auto *p = std::get_if<model::ParagraphBlock>(&section.blocks[0]);
  REQUIRE(p != nullptr);
  REQUIRE(detail::docstring_to_text(p->paragraph) == "mytool \\- A cool tool.");
}

TEST_CASE("make_synopsis_section for command with options only", "[manpage]") {
  model::Option opt{};
  opt.names = {"--verbose"};
  opt.doc = {"Verbose."};
  opt.type = model::ScalarType::Bool;

  std::vector<model::Argument> args = {opt};
  auto section = make_synopsis_section("mytool", args, false);
  REQUIRE(section.name == "SYNOPSIS");
  REQUIRE(section.blocks.size() == 1);
  auto *p = std::get_if<model::ParagraphBlock>(&section.blocks[0]);
  REQUIRE(p != nullptr);
  // Should contain "mytool" and "[OPTIONS]"
  auto text = detail::docstring_to_text(p->paragraph);
  REQUIRE(text.find("mytool") != std::string::npos);
  REQUIRE(text.find("[OPTIONS]") != std::string::npos);
}

TEST_CASE("make_synopsis_section for command with positional", "[manpage]") {
  model::Positional pos{};
  pos.name = "file";
  pos.doc = {"A file."};
  pos.type = model::ScalarType::File;

  std::vector<model::Argument> args = {pos};
  auto section = make_synopsis_section("mytool", args, false);
  auto *p = std::get_if<model::ParagraphBlock>(&section.blocks[0]);
  REQUIRE(p != nullptr);
  auto text = detail::docstring_to_text(p->paragraph);
  REQUIRE(text.find("FILE") != std::string::npos);
}

TEST_CASE("make_synopsis_section for command with subcommands", "[manpage]") {
  auto section =
      make_synopsis_section("mytool", std::vector<model::Argument>{}, true);
  auto *p = std::get_if<model::ParagraphBlock>(&section.blocks[0]);
  REQUIRE(p != nullptr);
  auto text = detail::docstring_to_text(p->paragraph);
  REQUIRE(text.find("COMMAND") != std::string::npos);
}

TEST_CASE("make_synopsis_section for command with options and subcommands", "[manpage]") {
  model::Flag flag{};
  flag.names = {"--verbose"};
  flag.doc = {"Verbose."};

  std::vector<model::Argument> args = {flag};
  auto section = make_synopsis_section("mytool", args, true);
  auto *p = std::get_if<model::ParagraphBlock>(&section.blocks[0]);
  REQUIRE(p != nullptr);
  auto text = detail::docstring_to_text(p->paragraph);
  REQUIRE(text.find("[OPTIONS]") != std::string::npos);
  REQUIRE(text.find("COMMAND") != std::string::npos);
}

TEST_CASE("make_commands_section produces labeled list of subcommands", "[manpage]") {
  model::Command cmd1{};
  cmd1.name = "build";
  cmd1.doc = {"Build the project."};

  model::Command cmd2{};
  cmd2.name = "test";
  cmd2.doc = {"Run tests."};

  auto section = make_commands_section({cmd1, cmd2});
  REQUIRE(section.name == "COMMANDS");
  REQUIRE(section.blocks.size() == 2);
  auto *b1 = std::get_if<model::LabelTextBlock>(&section.blocks[0]);
  REQUIRE(b1 != nullptr);
  REQUIRE(b1->label.find("build") != std::string::npos);
  auto *b2 = std::get_if<model::LabelTextBlock>(&section.blocks[1]);
  REQUIRE(b2 != nullptr);
  REQUIRE(b2->label.find("test") != std::string::npos);
}

// ---------------------------------------------------------------------------
// Phase 6: Auto-generated sections (EXIT STATUS, ENVIRONMENT, SEE ALSO)
// ---------------------------------------------------------------------------

TEST_CASE("make_exit_status_section produces labeled list", "[manpage]") {
  std::vector<model::ExitInfo> exits = {
      {0, std::nullopt, {"Success."}},
      {1, std::nullopt, {"General error."}},
  };
  auto section = make_exit_status_section(exits);
  REQUIRE(section.name == "EXIT STATUS");
  REQUIRE(section.blocks.size() == 2);
  auto *b0 = std::get_if<model::LabelTextBlock>(&section.blocks[0]);
  REQUIRE(b0 != nullptr);
  REQUIRE(b0->label == "0");
  auto *b1 = std::get_if<model::LabelTextBlock>(&section.blocks[1]);
  REQUIRE(b1 != nullptr);
  REQUIRE(b1->label == "1");
}

TEST_CASE("make_exit_status_section with code range", "[manpage]") {
  std::vector<model::ExitInfo> exits = {
      {10, 20, {"Range error."}},
  };
  auto section = make_exit_status_section(exits);
  auto *b = std::get_if<model::LabelTextBlock>(&section.blocks[0]);
  REQUIRE(b != nullptr);
  REQUIRE(b->label == "10-20");
}

TEST_CASE("make_environment_section produces labeled list of env vars", "[manpage]") {
  std::vector<model::EnvInfo> envs = {
      {"HOME", model::DocString{"User home directory."}},
      {"EDITOR", std::nullopt},
  };
  auto section = make_environment_section(envs);
  REQUIRE(section.name == "ENVIRONMENT");
  REQUIRE(section.blocks.size() == 2);
  auto *b0 = std::get_if<model::LabelTextBlock>(&section.blocks[0]);
  REQUIRE(b0 != nullptr);
  REQUIRE(b0->label.find("HOME") != std::string::npos);
}

TEST_CASE("make_see_also_section produces comma-separated refs", "[manpage]") {
  std::vector<model::ManXref> xrefs = {
      {"git", 1},
      {"gitconfig", 5},
  };
  auto section = make_see_also_section(xrefs);
  REQUIRE(section.name == "SEE ALSO");
  REQUIRE(section.blocks.size() == 1);
  auto *p = std::get_if<model::ParagraphBlock>(&section.blocks[0]);
  REQUIRE(p != nullptr);
  auto text = detail::docstring_to_text(p->paragraph);
  REQUIRE(text.find("git\\fR(1)") != std::string::npos);
  REQUIRE(text.find("gitconfig\\fR(5)") != std::string::npos);
}

// ---------------------------------------------------------------------------
// Phase 7: Section ordering and assembly
// ---------------------------------------------------------------------------

TEST_CASE("section_order returns correct relative ordering", "[manpage]") {
  REQUIRE(section_order("NAME") < section_order("SYNOPSIS"));
  REQUIRE(section_order("SYNOPSIS") < section_order("DESCRIPTION"));
  REQUIRE(section_order("DESCRIPTION") < section_order("COMMANDS"));
  REQUIRE(section_order("COMMANDS") < section_order("ARGUMENTS"));
  REQUIRE(section_order("ARGUMENTS") < section_order("OPTIONS"));
  REQUIRE(section_order("OPTIONS") < section_order("EXIT STATUS"));
  REQUIRE(section_order("EXIT STATUS") < section_order("ENVIRONMENT"));
  REQUIRE(section_order("ENVIRONMENT") < section_order("SEE ALSO"));
}

TEST_CASE("section_order for unknown section returns high value", "[manpage]") {
  REQUIRE(section_order("CUSTOM") > section_order("SEE ALSO"));
}

TEST_CASE("assemble for minimal root produces NAME and SYNOPSIS", "[manpage]") {
  model::Root root{};
  root.name = "mytool";
  root.doc = {"A test tool."};

  auto sections = assemble(root);
  REQUIRE(sections.size() >= 2);
  REQUIRE(sections[0].name == "NAME");
  REQUIRE(sections[1].name == "SYNOPSIS");
}

TEST_CASE("assemble for root with args includes arg sections", "[manpage]") {
  model::Root root{};
  root.name = "mytool";
  root.doc = {"A test tool."};

  model::Flag flag{};
  flag.names = {"--verbose"};
  flag.doc = {"Verbose output."};
  root.args = std::vector<model::Argument>{flag};

  auto sections = assemble(root);
  bool has_options = false;
  for (const auto &s : sections) {
    if (s.name == "OPTIONS")
      has_options = true;
  }
  REQUIRE(has_options);
}

TEST_CASE("assemble for root with commands includes COMMANDS section", "[manpage]") {
  model::Root root{};
  root.name = "mytool";
  root.doc = {"A test tool."};

  model::Command cmd{};
  cmd.name = "build";
  cmd.doc = {"Build."};
  root.commands = std::vector<model::Command>{cmd};

  auto sections = assemble(root);
  bool has_commands = false;
  for (const auto &s : sections) {
    if (s.name == "COMMANDS")
      has_commands = true;
  }
  REQUIRE(has_commands);
}

TEST_CASE("assemble for root with user sections interleaves them", "[manpage]") {
  model::Root root{};
  root.name = "mytool";
  root.doc = {"A test tool."};
  root.man = model::Man{};
  root.man->sections = std::vector<model::ManSection>{
      {"DESCRIPTION", {model::ParagraphBlock{{"A longer description."}}}},
  };

  auto sections = assemble(root);
  bool has_desc = false;
  for (const auto &s : sections) {
    if (s.name == "DESCRIPTION")
      has_desc = true;
  }
  REQUIRE(has_desc);
  // DESCRIPTION should come after SYNOPSIS
  int synopsis_idx = -1;
  int desc_idx = -1;
  for (int i = 0; i < static_cast<int>(sections.size()); ++i) {
    if (sections[i].name == "SYNOPSIS")
      synopsis_idx = i;
    if (sections[i].name == "DESCRIPTION")
      desc_idx = i;
  }
  REQUIRE(synopsis_idx < desc_idx);
}

TEST_CASE("assemble for root with exits, envs, xrefs", "[manpage]") {
  model::Root root{};
  root.name = "mytool";
  root.doc = {"A test tool."};
  root.exits = std::vector<model::ExitInfo>{{0, std::nullopt, {"Success."}}};
  root.envs = std::vector<model::EnvInfo>{{"HOME", model::DocString{"Home dir."}}};
  root.man = model::Man{};
  root.man->xrefs = std::vector<model::ManXref>{{"git", 1}};

  auto sections = assemble(root);
  bool has_exit = false;
  bool has_env = false;
  bool has_see = false;
  for (const auto &s : sections) {
    if (s.name == "EXIT STATUS")
      has_exit = true;
    if (s.name == "ENVIRONMENT")
      has_env = true;
    if (s.name == "SEE ALSO")
      has_see = true;
  }
  REQUIRE(has_exit);
  REQUIRE(has_env);
  REQUIRE(has_see);
}

// ---------------------------------------------------------------------------
// Phase 8: Full page rendering and integration
// ---------------------------------------------------------------------------

TEST_CASE("render_page starts with .TH header", "[manpage]") {
  std::vector<model::ManSection> sections = {
      {"NAME", {model::ParagraphBlock{{"mytool \\- a tool"}}}},
  };
  auto page = groff::render_page("mytool", 1, "1.0.0", sections);
  REQUIRE(page.find(".TH") == 0);
  REQUIRE(page.find("MYTOOL") != std::string::npos);
  REQUIRE(page.find("1") != std::string::npos);
  REQUIRE(page.find(".SH NAME") != std::string::npos);
}

TEST_CASE("to_groff for minimal root produces valid groff", "[manpage]") {
  model::Root root{};
  root.name = "mytool";
  root.doc = {"A simple tool."};
  root.version = "1.0.0";

  auto output = to_groff(root);
  REQUIRE(output.find(".TH") == 0);
  REQUIRE(output.find("MYTOOL") != std::string::npos);
  REQUIRE(output.find(".SH NAME") != std::string::npos);
  REQUIRE(output.find(".SH SYNOPSIS") != std::string::npos);
  REQUIRE(output.find("mytool \\\\- A simple tool.") != std::string::npos);
}

TEST_CASE("to_groff for realistic root produces complete man page", "[manpage]") {
  model::Root root{};
  root.name = "mytool";
  root.doc = {"A realistic test tool."};
  root.version = "2.0.0";

  model::Flag flag{};
  flag.names = {"-v", "--verbose"};
  flag.doc = {"Enable verbose output."};

  model::Option opt{};
  opt.names = {"-o", "--output"};
  opt.doc = {"Output file."};
  opt.type = model::ScalarType::File;
  opt.docv = "FILE";

  model::Positional pos{};
  pos.name = "input";
  pos.doc = {"Input file."};
  pos.type = model::ScalarType::File;

  root.args = std::vector<model::Argument>{flag, opt, pos};

  model::Command cmd{};
  cmd.name = "build";
  cmd.doc = {"Build the project."};
  root.commands = std::vector<model::Command>{cmd};

  root.exits = std::vector<model::ExitInfo>{{0, std::nullopt, {"Success."}}};
  root.envs = std::vector<model::EnvInfo>{{"HOME", model::DocString{"User home."}}};
  root.man = model::Man{};
  root.man->xrefs = std::vector<model::ManXref>{{"git", 1}};
  root.man->sections = std::vector<model::ManSection>{
      {"DESCRIPTION", {model::ParagraphBlock{{"A longer description of mytool."}}}},
  };

  auto output = to_groff(root);
  REQUIRE(output.find(".TH") == 0);

  // Verify section ordering
  auto name_pos = output.find(".SH NAME");
  auto synopsis_pos = output.find(".SH SYNOPSIS");
  auto desc_pos = output.find(".SH DESCRIPTION");
  auto commands_pos = output.find(".SH COMMANDS");
  auto args_pos = output.find(".SH ARGUMENTS");
  auto options_pos = output.find(".SH OPTIONS");
  auto exit_pos = output.find(".SH EXIT STATUS");
  auto env_pos = output.find(".SH ENVIRONMENT");
  auto see_pos = output.find(".SH SEE ALSO");

  REQUIRE(name_pos != std::string::npos);
  REQUIRE(synopsis_pos != std::string::npos);
  REQUIRE(desc_pos != std::string::npos);
  REQUIRE(commands_pos != std::string::npos);
  REQUIRE(args_pos != std::string::npos);
  REQUIRE(options_pos != std::string::npos);
  REQUIRE(exit_pos != std::string::npos);
  REQUIRE(env_pos != std::string::npos);
  REQUIRE(see_pos != std::string::npos);

  REQUIRE(name_pos < synopsis_pos);
  REQUIRE(synopsis_pos < desc_pos);
  REQUIRE(desc_pos < commands_pos);
  REQUIRE(commands_pos < args_pos);
  REQUIRE(args_pos < options_pos);
  REQUIRE(options_pos < exit_pos);
  REQUIRE(exit_pos < env_pos);
  REQUIRE(env_pos < see_pos);
}
