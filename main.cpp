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
   auto default_color = fmt::text_style();
   auto prefix_color = fg(fmt::color::magenta);
   auto line_num_color = fg(fmt::color::lime_green);
   auto match_color = fg(fmt::color::red);
   auto semicolon_color = fg(fmt::color::cyan);

   fmt::text_style style = default_color;

   if (!prefix.empty()) {
      if (flags.use_color) {
         style = prefix_color;
      }
      fmt::print(style, "{}", prefix);
   
      if (flags.use_color) {
         style = semicolon_color;
      }
      fmt::print(style, ":");
   }

   if (flags.print_line_nums) {
      style = default_color;

      if (flags.use_color) {
         style = line_num_color;
      }
      fmt::print(style, "{}", index);

      if (flags.use_color) {
         style = semicolon_color;
      }
      fmt::print(style, ":");    
   }

   if (flags.use_color) {
      style = default_color;
      fmt::print(style, "{}", line.substr(0, start));
      style = match_color;
      fmt::print(style, "{}", line.substr(start , end - start));
      style = default_color;
      fmt::print(style, "{}\n", line.substr(end, line.length() - end));
   }
   else {
      fmt::print("{}\n", line);
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

///////////////////////////////////////////////
//template<string LIMIT>
class Rambo {
   struct Line {
      bool valid;
      string str;
   };

   private:
      int tool;
      friend class Iterator;

   public:
      Rambo(int tool_) {
         tool = tool_;
      }

      class Iterator: public std::iterator<std::input_iterator_tag, string, long, const string*, string> {
         private:
            Line line;
            Rambo* brambo;
         public:
            explicit Iterator(Rambo* r) {
               line = {.valid = false};
               brambo = r;
            }
        
            Iterator& operator++() { 
               if (std::getline(std::cin, line.str)) {
                  line.valid = true;
                  std::stringstream linestream(line.str);
                  fmt::print("tool: {}\n", brambo->tool);
               }

               return *this;
            }
        
            Iterator operator++(int) {
               Iterator retval = *this;
               ++(*this);
               return retval;
            }

            bool operator==(Iterator other) const {
               return line.str == other.line.str;
            }

            bool operator!=(Iterator other) {
               if ((line.str != "") || !line.valid) {
                  return true;
               }
               else {
                  return false;
               }

               line.valid = false;
            }

            reference operator*() const {
               return line.str;
            }

      };

   void Up() {
      tool++;
   }

   Iterator begin() {
      return Iterator(this);
   }

   Iterator end() {
      return Iterator(this);
   }
};
///////////////////////////////////////////////

//////////////////////////////////////////////
template<long LIMIT>
class Rando {
   public:
      class Iterator: public std::iterator<std::input_iterator_tag, long, long, const long*, long> {
         private:
            long num;
            long count;

         public:
            explicit Iterator() {
               num= 0;
               count = 0;
               srand((unsigned)time(0));
            }
        
            Iterator& operator++() { 
               num = rand() % 500;
               count++;
               return *this;
            }
        
            Iterator operator++(int) {
               Iterator retval = *this;
               ++(*this);
               return retval;
            }

            bool operator==(Iterator other) const {
               return num == other.num;
            }

            bool operator!=(Iterator other) const {
               if (count < LIMIT) {
                  return true;
               }
               else {
                  return false;
               }
            }

            reference operator*() const {
               return num;
            }

      };

   Iterator begin() {
      return Iterator();
   }

   Iterator end() {
      return Iterator();
   }
};
//////////////////////////////////////////////


int main(int argc, const char *argv[]) {
   std::unique_ptr<Config> config = parse_args(argc, argv);

   if (config == nullptr) {
      exit(0);
   }

   print_matched_line(config->flags, "test.tst", 0, "This is a test string...", 3, 10);

   fmt::print("files.len: {}\n", config->files.size());

   fmt::print("config.pattern: {}\n", config->pattern);

   auto r = Rambo(42);
   for (auto a: r) {
      r.Up();
      std::cout << a << endl;
   }

   //    for (auto w : sayHelloToWorld())
   // {
   //     std::cout << w;
   // }
}
