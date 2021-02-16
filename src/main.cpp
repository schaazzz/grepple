#include <iostream>
#include <vector>
#include <atomic>

#include <boost/program_options.hpp>
#include <boost/fiber/all.hpp>

#include <fmt/core.h>
#include <fmt/color.h>

#include "line_grep.h"

using namespace boost::program_options;
using namespace boost::fibers;

using namespace std::chrono_literals;

using std::atomic;
using std::vector;

static inline const std::string VERSION = "0.1.0";
static inline const std::string AUTHOR = "Shahzeb Ihsan <shahzeb@gmail.com>";
static inline const std::string DESCRIPTION = "Gre++le: An academic exercise to develop a GREP clone in C++20";

typedef struct {
   bool ignore_case;
   bool print_line_nums;
   bool use_color;
} Flags;

typedef struct {
   Flags flags;
   std::string pattern;
   vector<std::string> files;
} Config;

static void print_usage(options_description& flags, bool print_about) {
   fmt::print("---\n");

   if (print_about) {
      fmt::print("{}, v{}\n", DESCRIPTION, VERSION);
      fmt::print("{}\n", AUTHOR);
      fmt::print("\n");
   }

   std::cout << flags << std::endl;
   fmt::print("---\n");
}

static void print_matched_line(Flags& flags, std::string prefix, int index, std::string line, int start, int end) {
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

///////////////////////////////////////////////
//template<string LIMIT>
class Rambo {
   struct Line {
      bool valid;
      std::string str;
   };

   private:
      int tool;
      friend class Iterator;

   public:
      Rambo(int tool_) {
         tool = tool_;
      }

      class Iterator: public std::iterator<std::input_iterator_tag, Rambo::Line, long, const Rambo::Line*, Rambo::Line> {
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
               return line;
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

struct LineSource {
};

static void process(LineSource source, Flags& flags, std::string pattern) {
   typedef boost::fibers::condition_variable cv;
   typedef boost::fibers::buffered_channel<std::tuple<uint32_t, uint32_t>> chan_res;
   typedef boost::fibers::buffered_channel<std::string> chan_line;
   
   atomic<bool> exit(false);
   chan_res main_rx{2};
   chan_line main_tx{2};

   auto grep = LineGrep::build(pattern, flags.ignore_case);
   if (grep == std::nullopt) {
      fmt::print(stderr, "Error: Invalid regular expression!\n");
      return;
   }

   fiber fb(
      std::bind(
         [&] (chan_res& tx, chan_line& rx, atomic<bool>& exit) {
            std::string line = "";
            while(true) {
               if (boost::fibers::channel_op_status::success == rx.pop_wait_for(line, 5ms)) {
                  fmt::print("go fuckyourself\n");
                  tx.push(grep->search(line));
               }

               if(exit.load()) {
                  break;
               }
            }
         },
         std::ref(main_rx),
         std::ref(main_tx),
         std::ref(exit)
      )
   );

   auto r = Rambo(42);
   std::tuple<uint32_t, uint32_t> res;
   for (auto a: r) {
      if (a.valid) {
         main_tx.push(a.str);
         main_rx.pop(res);
         print_matched_line(flags, "", 0, a.str, std::get<0>(res), std::get<1>(res));
      }
   }

   exit = true;
   fb.join();
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
      ("pattern", value<std::string>(), "p p p")
      ("files", value<vector<std::string>>(), "f f f");
   
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
      config.pattern = vm["pattern"].as<std::string>();
   }
   else {
      fmt::print("Error: Pattern cannot be empty!\n");
      print_usage(flags, false);
      return std::unique_ptr<Config>(nullptr);
   }

   if (vm.count("files")) {
      config.files = vm["files"].as<vector<std::string>>();
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
   
   if (config->files.empty) {
      LineSource source;
      process(source, config->flags, config->pattern);
   }
   else {
      std::cout << config->files << std::endl;
   }
}
