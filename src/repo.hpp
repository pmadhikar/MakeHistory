#ifndef REPO_H_
#define REPO_H_

extern "C"
{
  #include"git2.h"
}

#include<string>
#include<memory>
#include<unordered_map>
#include<format>
#include<stdlib.h>

namespace detail
{
  git_repository* open_repo(std::string_view path)
  {
    git_repository* repo;
    int error = git_repository_open(&repo, path.data());

    if (error)
    {
      const git_error *e = git_error_last();
      throw std::runtime_error(std::format("No git repo at {}. Git Error: {}/{}: {}", path, error, e->klass, e->message));
    }

    return repo;
  }

  git_revwalk* get_commit_walker(git_repository* repo)
  {
    git_revwalk* walker;
    int error = git_revwalk_new(&walker, repo);

    if (error<0)
    {
      const git_error *e = git_error_last();
      throw std::runtime_error(std::format("Could not open walker {}", e->message));
    }

    git_revwalk_sorting(walker, GIT_SORT_TIME | GIT_SORT_TOPOLOGICAL);
    if (git_revwalk_push_head(walker) < 0)
    {
      const git_error *e = git_error_last();
      throw std::runtime_error(std::format("Could not push head {}", e->message));
    }

    return walker;
  }

  git_commit* get_commit(git_revwalk* walker, git_oid* oid, git_repository* repo)
  {
    git_revwalk_next(oid, walker);
    git_commit* commit;
    git_commit_lookup(&commit, repo, oid);

    return commit;
  }
}

struct Git {
  private:
    Git()
    {
      std::println("Initializing libgit2.");
      git_libgit2_init();
    }
  public:
    ~Git()
    {
      std::println("All done. Cleaning up libgit2.");
      git_libgit2_shutdown();
    }
    struct Repo;


    struct RevWalker
    {
      std::unique_ptr<git_revwalk, decltype(&git_revwalk_free)> walker;
      RevWalker(): walker{nullptr, git_revwalk_free} {};
      RevWalker(Repo& repo): walker{detail::get_commit_walker(repo.repo.get()), git_revwalk_free} {};
    };

    struct Commit
    {
      git_oid oid;
      std::unique_ptr<git_commit, decltype(&git_commit_free)> commit;
      std::unique_ptr<const char> commit_msg{};

      std::string sha{};
      std::string message{};

      Commit(Repo& r, RevWalker& w):
        commit{detail::get_commit(w.walker.get(), &oid, r.repo.get()), git_commit_free}{

        std::array<char, GIT_OID_HEXSZ+1> sha_array{};
        git_oid_fmt(sha_array.data(), &oid);
        sha_array[GIT_OID_HEXSZ] = '\0';

        sha = sha_array.data();

        message = git_commit_message(commit.get());
      }

    };

    struct Repo {

      std::string_view repo_path{};
      std::unique_ptr<git_repository, decltype(&git_repository_free)> repo;
      RevWalker walker{};

      Repo(std::string_view repo_path_): repo_path{}, repo{detail::open_repo(repo_path_), git_repository_free}{
        walker = RevWalker(*this);
      }

      void listCommits(){
        auto c = Commit(*this, walker);
        std::println("{}\n{}", c.sha, c.message);
      };
    };

    static Repo& gimmeRepo(std::string_view repoPath){
      static Git git{};
      static std::unordered_map<std::string_view, Repo> repos{};

      if (!repos.contains(repoPath))
      {
        repos.insert(std::make_pair(repoPath, Repo{repoPath}));
      }
      return repos.at(repoPath);
    }
};

// template<>
// struct std::formatter<Git::Commit>
// {
//   formatter() {};

//   constexpr auto parse(std::format_parse_context& ctx){
//     return ctx.begin();
//   }

//   auto format(const Git::Commit& commit, std::format_context& ctx)
//   {
//     return std::format_to(ctx.out(), "sha:{}\n{}", commit.sha, commit.message);
//   }
// };

// template <>
// struct std::formatter<Git::Commit> : std::formatter<std::string_view> {
//     auto format(const Git::Commit& commit, std::format_context& ctx) const {
//         std::string temp;
//         std::format_to(std::back_inserter(temp), "{}\n{}", commit.sha, commit.message);

//         return std::formatter<std::string_view>::format(temp, ctx);
//     }
// };


#endif // REPO_H_
