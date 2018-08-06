#include "coddle/config.hpp"

void configure(Config &cfg)
{
  cfg.common.libs.push_back("GL");
  cfg.common.cflags.push_back("-D_GLIBCXX_DEBUG");

  cfg.getLib(cfg, "sdlpp", "master");

  // git clone shade library
  // to quckly itarate
  cfg.common.incDirs.push_back("shade");
  cfg.addProject(cfg, "shade", TargetType::StaticLib);

  // to use git version do
  // cfg.getLib(cfg, "shade", "master");

  cfg.addProject(cfg, ".", TargetType::Executable);
}
