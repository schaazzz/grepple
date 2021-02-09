#include <iostream>
#include <vector>

#include <boost/program_options.hpp>
#include <fmt/core.h>
#include <fmt/color.h>

//import line_grep;
//import line_src;

using namespace std;
using namespace boost::program_options;

static inline const string VERSION = "0.1.0";
static inline const string AUTHOR = "Shahzeb Ihsan <shahzeb@gmail.com>";
static inline const string DESCRIPTION = "Gre++le: An academic exercise to develop a GREP clone in C++20";

typedef struct {
   bool ignore_case;
   bool print_line_nums;
   bool use_color;
} Flags;

typedef struct {
   Flags flags;
   string pattern;
   vector<string> files;
} Config;

static void print_usage(options_description& flags, bool print_about) {
   fmt::print("---\n");

   if (print_about) {
      fmt::print("{}, v{}\n", DESCRIPTION, VERSION);
      fmt::print("{}\n", AUTHOR);
      fmt::print("\n");
   }

   cout << flags << endl;
   fmt::print("---\n");

}

static void print_matched_line(Flags& flags, string prefix, int index, string line, int start, int end) {
   auto prefix_color = fg(fmt::color::magenta);
   auto line_num_color = fg(fmt::color::green);
   auto match_color = fg(fmt::color::red);
   auto semicolon_color = fg(fmt::color::cyan);

   fmt::print("color: {}\n", flags.use_color);

   if (!prefix.empty()) {
      fmt::text_style style;

      if (flags.use_color) {
         style = prefix_color;
      }
      fmt::print(style, "{}", prefix);
   
      if (flags.use_color) {
         style = semicolon_color;
      }
      fmt::print(style, ":\n");
   }
}

static std::unique_ptr<Config> parse_args(int argc, const char *argv[]) {
   options_description flags {
      "Usage: rusty_grep [OPTION]... PATTERNS [FILE]...\n"
      "\n"
      "Options"};

   flags.add_options()
      ("i,i", "ignore case distinctions in patterns and data")
      ("n,n", "print line number with output lines")
      ("color", "use markers to highlight the matching strings")
      ("help", "print this help menu");


   options_description hidden {""};
   hidden.add_options()
      ("pattern", value<string>(), "p p p")
      ("files", value<vector<string>>(), "f f f");
   
   options_description all {""};
   all.add(flags).add(hidden);

   positional_options_description pos;
   pos.add("pattern", 1);
   pos.add("files", -1);

   variables_map vm;
   store(command_line_parser(argc, argv).options(all).positional(pos).run(), vm);
   notify(vm);

   Config config = { .pattern =  "" };

   if (vm.count("help")) {
      print_usage(flags, true);
      return std::unique_ptr<Config>(nullptr);
   }

   if (vm.count("pattern")) {
      config.pattern = vm["pattern"].as<string>();
   }
   else {
      fmt::print("Error: Pattern cannot be empty!\n");
      print_usage(flags, false);
      return std::unique_ptr<Config>(nullptr);
   }

   if (vm.count("files")) {
      config.files = vm["files"].as<vector<string>>();
   }

   if (vm.count("i")) {
      config.flags.ignore_case = true;
   }

   if (vm.count("n")) {
      config.flags.print_line_nums = true;
   }

   if (vm.count("color")) {
      config.flags.use_color = true;
   }

   return std::make_unique<Config>(config);
}

int main(int argc, const char *argv[]) {
   std::unique_ptr<Config> config = parse_args(argc, argv);

   if (config == nullptr) {
      exit(0);
   }

   print_matched_line(config->flags, "test.tst", 0, "", 0, 0);

   fmt::print("files.len: {}\n", config->files.size());

   fmt::print("config.pattern: {}\n", config->pattern);
}
