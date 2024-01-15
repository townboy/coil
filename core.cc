#include "mdspan.hpp"
#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>

using Mem = std::unique_ptr<uint8_t[]>;
using Mdspan2d = std::experimental::mdspan<
    uint8_t, std::experimental::extents<uint8_t, std::dynamic_extent,
                                        std::dynamic_extent>>;

enum class Dir : uint8_t { UP, RIGHT, DOWN, LEFT };

// x = height, y = width
struct StartPoint {
  uint8_t x;
  uint8_t y;
};

struct Maze : public Mem, public Mdspan2d {
  constexpr static std::array<std::array<int, 2>, 4> dir{-1, 0, 0, 1,
                                                         1,  0, 0, -1};
  constexpr static std::array<char, 4> dir_name{'U', 'R', 'D', 'L'};
  struct Solution {
    StartPoint sp;
    std::list<Dir> path;
  };
  Maze(std::size_t width, std::size_t height, std::string map_str)
      : Mem(new uint8_t[(width + 2) * (height + 2)]),
        Mdspan2d(Mem::get(), std::experimental::extents{height + 2, width + 2}),
        height(height), width(width), total_size((width + 2) * (height + 2)) {
    parse_mapstr(map_str);
  }

  // 这是题目的格式
  // var width = 5; var height = 3; var boardStr = "......X......X."
  // GET /coil/?x=4&y=2&path=ULDRUR
  //题目的答案横竖索引从0开始
  //.....
  //.X...
  //...X.

  void parse_mapstr(std::string map_str) {
    /*
    为了方便处理，所以在周围一圈加上一圈障碍物，如下
    ######
    #....#
    #....#
    ######
    */
    std::fill(get(), get() + total_size, 1);
    auto count = 0u;
    assert(map_str.size() == height * width);
    for (auto i = 1; i <= height; i++) {
      for (auto f = 1; f <= width; f++) {
        operator()(i, f) = map_str[count++] == 'X' ? 1 : 0;
      }
    }
  }

  void display_map() {
    // std::cout.setf(std::ios_base::hex, std::ios_base::basefield);
    for (auto i = 0; i < height + 2; i++) {
      for (auto f = 0; f < width + 2; f++) {
        // std::cout.width(4);
        std::cout << (operator()(i, f) ? '#' : '.');
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
    // std::cout.unsetf(std::ios_base::hex);
  }

  bool check_solution(Solution s) {
    Mem map(new uint8_t[total_size]);
    std::experimental::mdspan span(
        map.get(), std::experimental::extents(height + 2, width + 2));
    std::copy(get(), get() + total_size, map.get());

    assert(!span(s.sp.x, s.sp.y));
    span(s.sp.x, s.sp.y) = 1;
    auto nx = 0;
    auto ny = 0;
    for (auto d : s.path) {
      auto iter = [d, &s, this, &nx, &ny]() {
        auto di = static_cast<uint8_t>(d);
        nx = s.sp.x + dir[di][0];
        ny = s.sp.y + dir[di][1];
      };
      while (iter(), !span(nx, ny)) {
        span(nx, ny) = 1;
        s.sp.x = nx;
        s.sp.y = ny;
      }
    }

    for (auto i = 0; i < height + 2; i++) {
      for (auto f = 0; f < width + 2; f++) {
        // std::cout.width(4);
        std::cout << (span(i, f) ? '#' : '.');
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;

    // std::cout.unsetf(std::ios_base::hex);

    return std::all_of(map.get(), map.get() + total_size,
                       [](uint8_t v) { return v; });
  }

  std::size_t height;
  std::size_t width;
  std::size_t total_size;
};

template <> class std::hash<StartPoint> {
public:
  std::size_t operator()(StartPoint const &sp) {
    std::hash<uint16_t> h;
    return h((sp.x << 8) + sp.y);
  }
};

struct PathCollection {};

std::unique_ptr<Maze> maze;
std::unordered_map<StartPoint, PathCollection> saved_status;

int main() {
  std::cout << "solving puzzle" << std::endl;
  std::size_t width;
  std::size_t height;
  std::string map_str;
  std::cin >> width >> height >> map_str;
  maze = std::make_unique<Maze>(width, height, map_str);
  maze->display_map();
  std::cout << "check solution " << std::endl
            << maze->check_solution(
                   {1,
                    2,
                    {Dir::RIGHT, Dir::DOWN, Dir::LEFT, Dir::DOWN, Dir::RIGHT}})
            << std::endl;
  return 0;
}
