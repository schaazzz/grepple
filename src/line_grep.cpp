#include <iostream>
#include <string>

#include <fmt/core.h>

#include "line_grep.h"

LineGrep::LineGrep(std::string pattern, bool ignore_case) {
    boost::regex::flag_type flags = 
        boost::regex_constants::perl |
        boost::regex_constants::no_except |
        (ignore_case? boost::regex_constants::icase : 0);

    re_ = std::unique_ptr<boost::regex>(new boost::regex(pattern, flags));
    
    if (re_->empty()) {
        throw(std::runtime_error("Error: Invalid regular expression!"));
    }
}

std::tuple<uint32_t, uint32_t> LineGrep::search(std::string& line) {
    boost::smatch match;
    if (boost::regex_search(line, match, *re_)) {
        return {match.position(), match.position() + match.length()};
    }
    else {
        return {0, 0};
    }
}
