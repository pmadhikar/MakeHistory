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
#include<print>
#include<functional>

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

    git_revwalk_sorting(walker, GIT_SORT_TIME | GIT_SORT_TOPOLOGICAL | GIT_SORT_REVERSE);
    if (git_revwalk_push_head(walker) < 0)
    {
      const git_error *e = git_error_last();
      throw std::runtime_error(std::format("Could not push head {}", e->message));
    }

    return walker;
  }

  std::optional<git_oid> get_next_commit_oid(git_revwalk* walker, git_repository* repo)
  {
    git_oid oid{};

    if (git_revwalk_next(&oid, walker) < 0){
      return std::nullopt;
    }

    return oid;
  }

  void walk_commit_oids(git_revwalk* walker, git_repository* repo, std::function<void(git_oid)> callback)
  {
    for (std::optional<git_oid> maybe_oid = get_next_commit_oid(walker, repo);
         maybe_oid.has_value();
         maybe_oid = get_next_commit_oid(walker, repo)){
      callback(*maybe_oid);
    }
  }

  git_commit* get_git_commit(git_repository* repo, git_oid* oid)
  {
    git_commit* commit;

    if (git_commit_lookup(&commit, repo, oid) < 0){
      const git_error *e = git_error_last();
      throw std::runtime_error(std::format("Could not look commit up {}", e->message));
    }

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
    struct Repo {

      struct RevWalker
      {
        std::unique_ptr<git_revwalk, decltype(&git_revwalk_free)> walker;
        RevWalker(): walker{nullptr, git_revwalk_free} {};
        RevWalker(Repo& repo): walker{detail::get_commit_walker(repo.get()), git_revwalk_free} {};

        git_revwalk* get() { return walker.get();}
      };

      struct Commit
      {
        git_oid oid;
        std::unique_ptr<git_commit, decltype(&git_commit_free)> commit;

        std::string sha{};
        std::string message{};

        Commit(Repo& repo_, git_oid oid_): oid{oid_}, commit{detail::get_git_commit(repo_.get(), &oid), git_commit_free}{
          std::array<char, GIT_OID_HEXSZ+1> sha_array{};
          git_oid_fmt(sha_array.data(), &oid);
          sha_array[GIT_OID_HEXSZ] = '\0';
          sha = sha_array.data();
          message = git_commit_message(commit.get()); // leaks memory
        }
      };

      std::string_view repo_path{};
      std::unique_ptr<git_repository, decltype(&git_repository_free)> repo;
      RevWalker walker{};
      std::string_view prefix;
      std::vector<Commit> marked_commits{};

      Repo(std::string_view repo_path_, std::string_view prefix_): repo_path{}, repo{detail::open_repo(repo_path_), git_repository_free}, prefix{prefix_}{
        walker = RevWalker(*this);
        populate();
      }

      void populate()
      {
        std::println("Searching for commits with marker \"{}\"", prefix);

        auto add_to_commits = [this](git_oid oid)
        {
          Commit c{*this, oid};
          println("commit:{}\nmessage:{}", c.sha, c.message);
          if (c.message.starts_with(prefix))
          {
            this->marked_commits.emplace_back(std::move(c));
          }
        };

        detail::walk_commit_oids(walker.get(), repo.get(), add_to_commits);
      }

      void listCommits(){
        // auto c = Commit(*this, walker);
        // std::println("{}\n{}", c.sha, c.message);
      };

      git_repository* get() { return repo.get();}
    };

    static Repo& GimmeRepo(std::string_view repoPath, std::string_view prefix = ""){
      static Git git{};
      static std::unordered_map<std::string_view, Repo> repos{};

      if (!repos.contains(repoPath))
      {
        repos.insert(std::make_pair(repoPath, Repo{repoPath, prefix}));
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
