export module level;
import atlas;
import enemies;
import plane;

static constexpr const char ground_data[plane::t::h][plane::t::w + 1] {
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0011111111111111",
  "0011122222221111",
  "0011112222221111",
  "0001122222211000",
  "0000122222210000",
  "0000122222110000",
  "0001111111100000",
  "0000111111110000",
  "0000111111111111",
  "0011111100011111",
  "0111100000111100",
  "0111100000111000",
};
static constexpr const plane::t ground_plane = [] {
  plane::t res {};
  for (auto y = 0; y < plane::t::h; y++) {
    for (auto x = 0; x < plane::t::w; x++) {
      auto & p = res.at({ x, y });
      switch (ground_data[y][x]) {
        case '0': p = plane::at_water; break;
        case '1': p = plane::at_grass; break;
        case '2': p = plane::at_dirt; break;
        default: throw 0;
      }
    }
  }
  return res;
}();

static constexpr const char sky_data[plane::t::h][plane::t::w + 1] {
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0001000000000000",
  "0000010000000000",
  "0001000000000000",
  "0000010000000000",
  "0001000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000111000000000",
  "0000000000000000",
  "0000000000011100",
  "0000000000000000",
  "0000000000000000",
  "0000000001111100",
  "0000000011111000",
  "0000000000011111",
  "0000000000000000",
  "0111000000000000",
  "0011100000000000",
  "0001111100000000",
  "0111111000000000",
  "0000000000000000",
  "0000000000000000",
};
static constexpr const plane::t sky_plane = [] {
  plane::t res {};
  for (auto y = 0; y < plane::t::h; y++) {
    for (auto x = 0; x < plane::t::w; x++) {
      auto & p = res.at({ x, y });
      switch (sky_data[y][x]) {
        case '0': p = plane::at_void; break;
        case '1': p = plane::at_cloud; break;
        default: throw 0;
      }
    }
  }
  return res;
}();

static void init_enemies() {
  enemies::reset([](auto * e) {
    constexpr const auto sy = atlas::min_area_y - 1;

    *e++ = { .spawn_time = 2, .s0 = { -0.5f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 2.5f, .s0 = { -0.5f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 3, .s0 = { -0.5f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 3.5f, .s0 = { -0.5f, sy }, .v0 = { 0, 5 } };

    *e++ = { .spawn_time = 5.0f, .s0 = { -5.f, sy }, .v0 = { 0, 5 }, .a = { 1, 0 } };
    *e++ = { .spawn_time = 5.5f, .s0 = { -5.f, sy }, .v0 = { 0, 5 }, .a = { 1, 0 } };
    *e++ = { .spawn_time = 6.0f, .s0 = { -5.f, sy }, .v0 = { 0, 5 }, .a = { 1, 0 } };
    *e++ = { .spawn_time = 6.5f, .s0 = { -5.f, sy }, .v0 = { 0, 5 }, .a = { 1, 0 } };

    *e++ = { .spawn_time = 8.0f, .s0 = { 5.f, sy }, .v0 = { 0, 5 }, .a = { -1, 0 } };
    *e++ = { .spawn_time = 8.5f, .s0 = { 5.f, sy }, .v0 = { 0, 5 }, .a = { -1, 0 } };
    *e++ = { .spawn_time = 9.0f, .s0 = { 5.f, sy }, .v0 = { 0, 5 }, .a = { -1, 0 } };
    *e++ = { .spawn_time = 9.5f, .s0 = { 5.f, sy }, .v0 = { 0, 5 }, .a = { -1, 0 } };
  });
}

export namespace level {
  void load() {
    atlas::planes::ground() = ground_plane;
    atlas::planes::sky() = sky_plane;
    init_enemies();
  }
}
