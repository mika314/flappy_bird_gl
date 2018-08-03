#include "coddle/config.hpp"

void configure(Config&cfg)
{
  cfg.common.libs.push_back("GL");
  cfg.common.cflags.push_back("-D_GLIBCXX_DEBUG");
}
