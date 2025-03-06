#ifndef REPO_H_
#define REPO_H_

extern "C"
{
  #include "git2.h"
}

#include <string>

struct Repo{
  std::string_view repo_path{};
  git_repository* repo{};
  int error{};

  Repo(std::string_view repo_path_): repo_path{repo_path_}{
    git_libgit2_init();
    int error = git_repository_open(&repo, repo_path.data());
    if (error)
    {
      std::println("Could not open git repo at {}", repo_path);
    }
  };

  ~Repo(){
    git_libgit2_shutdown();
  }
};

#endif // REPO_H_
