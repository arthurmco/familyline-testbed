#include <client/graphical/exceptions.hpp>

using namespace familyline::graphics;

graphical_exception::graphical_exception(std::string_view message)
    : std::runtime_error(message.data()), _message(message)
{
    msg_ = fmt::format("Graphical error {}", this->_message.data());
}
