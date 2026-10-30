#ifndef INPUT_ERROR_H
#define INPUT_ERROR_H

#include <exception>
#include <optional>
#include <string>

namespace MathEngine
{

struct SourceRange {
    size_t start, length;
};

class InputError : public std::exception
{
    std::string m_err;
    std::optional<SourceRange> m_range;

  public:
    InputError(const std::string &err,
               std::optional<SourceRange> range = std::nullopt) noexcept
        : m_err(err), m_range(range) {}

    const char *what() const noexcept override { return m_err.c_str(); }
    std::optional<SourceRange> range() const noexcept { return m_range; }
};

} // namespace MathEngine

#endif // INPUT_ERROR_H