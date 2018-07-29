#pragma once
#include <sstream>

#define GL_CHECK_ERROR(...) internal::checkError(__FILE__, __LINE__, __VA_ARGS__)

namespace internal
{
  void outputArgs(std::ostringstream &strm);

  template <typename T, typename... Args>
  void outputArgs(std::ostringstream &strm, T &&arg, Args &&... args)
  {
    strm << " " << arg;
    outputArgs(strm, args...);
  }

  bool checkGlError(const char *file, int line, std::ostream &strm);

  template <typename... Args>
  void checkError(const char *file, int line, Args &&... args)
  {
    std::ostringstream strm;
    if (!internal::checkGlError(file, line, strm))
      internal::outputArgs(strm, args...);
  }
} // namespace internal
