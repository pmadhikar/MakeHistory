#include<array>
#include<mdspan>
#include<string>
#include<iostream>
#include<map>

#include "images/hi.hpp"
#include "repo.hpp"

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

  std::map<std::string_view, std::string_view> arguments{};
  

  if (argc % 2 == 0)
  {
    std::println("usage: MakeHistory [arg_name] [arg_value]...\n");
  }

  for (int arg_count = 1; arg_count < argc; arg_count+=2)
  {
    std::string_view arg{argv[arg_count]};

    if (arg == "--path")
    {
      arguments["path"] = argv[arg_count+1];
    }
    else
    {
      std::println("Unused argument: {} {}", arg, argv[arg_count+1]);
    }
  }

  auto image_data = get_hi_image();

  std::mdspan<int, std::extents<size_t, 7, 50>> imageMap{image_data.data()};

  constexpr int defaultCommitsPerDay{30};

  constexpr std::string_view commitPrefix = "xXx69_MakingHistory_with_yourmomxXx: ";

  const int commitsPerDay = argc > 1 ? ::atoi(argv[1]): defaultCommitsPerDay;

  std::println("Writing:");
  for (int i = 0; i < 7; ++i)
  {
    for (int j = 0; j < 50; ++j)
    {
      imageMap[i, j] ? std::print("{}", imageMap[i,j]) : std::print(" ");
    }
    std::println("");
  }

  Repo gitRepo{arguments["path"]};

  return 0;
}
