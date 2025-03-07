#include<array>

#include<string>
#include<iostream>
#include<map>

#include "images/hi.hpp"
#include "git.hpp"
#include "images/ascii.hpp"
#include <print>
#include <filesystem>

auto main(int argc, char *argv[]) -> int {

  std::println(std::cout, "Let's make some history");

  /**
   * github uses the following grid to represent the dates on the calendar
   *       jan feb march ....
   *  sun
   *  mon
   *  tue
   *  ...
   *  sat
   *
   * the grid therefore has 7 rows, and 365/7 = 52 columns. now, 52*7 = 364 so we actually
   * have one more than element than 7 * 52. We are not smart enough for this, so we
   * will limit ourselves to 7 rows * 50 colums. Our images will ignore the first and last week,
   * and the annoying extra day.
   *
   * This means a matrix of 7 rows by 50 columns. Let's use row major storage and std::mdspan to make our
   * lives a little easier.
   *
   * In order to make the commits more visible, we'll be creating 30 commits per date by default, feel free to set a
   * custom number of commits to make this work as a command line argument.
   *
   */

  /**
   * argument parsing
   */

  std::map<std::string_view, std::string_view> config{
  {"path", "."},
  {"prefix", "xXx69_MakingHistory_with_yourmomxXx: "},
  {"commits-per-day", "30"}
  };
  
  if (argc % 2 == 0)
  {
    std::println("usage: MakeHistory [arg_name] [arg_value]...\n");
    return -1;
  }

  for (int arg_count = 1; arg_count < argc; arg_count+=2)
  {
    std::string_view arg{argv[arg_count]};

    auto value = [&argv, arg_count]()
    {
      return argv[arg_count+1];
    };

    if (arg == "--path")
    {
      config.at("path") = value();
    }
    else if(arg == "--prefix")
    {
      config.at("prefix") = value();
    }
    else if (arg == "--commits-per-day")
    {
      config.at("--commits-per-day") = value();
    }
    else
    {
      std::println("Unused argument: {} {}", arg, value());
    }
  }

  auto image_data = get_hi_image();

  std::println("Writing:");

  for (int i = 0; i < 7; ++i)
  {
    for (int j = 0; j < 50; ++j)
    {
      int c = image_data[50*i + j];
      c ? std::print("{}", c) : std::print(" ");
    }
    std::println("");
  }

  std::println("{}", config.at("path"));

  Git::Repo& repo = Git::GimmeRepo(config["path"], config["prefix"]);

  repo.listCommits();

  auto grid = *ascii::get_char_grid('#');

  for (auto row: grid.backing)
  {
    for (auto i : std::views::iota(0, grid.numCols))
    {
      row[i] ? std::print("x"): std::print(".");
    }
    std::println("");
  }

  return 0;
}
