#include <exception>
#include <sstream>
#include <string>


class GammaAPIError : public std::exception {
  public:
    template <class... Args>
    GammaAPIError(Args &&...args);

    char const *what() const noexcept override
    {
        return msg.c_str();
    }

  private:
    std::string msg;
};

template <class... Args>
GammaAPIError::GammaAPIError(Args &&...args)
{
    std::ostringstream sout;
    (sout << ... << args);
    this->msg = sout.str();
}