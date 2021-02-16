#ifndef __LINE_GREP_H__
#define __LINE_GREP_H__

#include <optional>
#include <boost/regex.hpp>

class LineGrep {
    public:
        template <typename... Args>
        static std::optional<LineGrep> build(Args&&... args) noexcept {
            try {
                return std::optional<LineGrep>(LineGrep(args...));
            }
            catch(...) {
                return std::nullopt;
            }
        }

        std::tuple<uint32_t, uint32_t> search(std::string& line);

    private:
        LineGrep(std::string, bool);

        std::unique_ptr<boost::regex> re_;
};

#endif  /* __LINE_GREP_H__ */
