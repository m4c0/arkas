#pragma leco app

import atlas;
import casein;
import enemies;
import plane;
import quack;
import rng;
import ships;

static void load_atlas() {
  for (auto y = 0; y < plane::t::h; y++) {
    for (auto x = 0; x < plane::t::w; x++) {
      atlas::ground({ x, y }) = plane::at_water;
    }
    const auto w = plane::t::w;
    atlas::ground({ static_cast<int>(rng::rand(w)), y }) = plane::at_grass;
    atlas::sky({ static_cast<int>(rng::rand(w)), y }) = plane::at_cloud;
  }
}
static void load_enemies() {
  enemies::reset([](auto * e) {
    constexpr const auto sy = atlas::min_area_y - 1;

    *e++ = { .spawn_time = 2, .s0 = { 0.f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 2.5f, .s0 = { 0.f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 3, .s0 = { 0.f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 3.5f, .s0 = { 0.f, sy }, .v0 = { 0, 5 } };

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

static float g_x = 0;
static float g_displ_y = 0;

static void tick() {
  enemies::move_enemies(g_displ_y);
  atlas::parallax(atlas::initial_displ_y - g_displ_y, { g_x, 0.f });

  enemies::blit();
}

static constexpr auto strafe(int dx) {
  return [=] { g_x += dx; };
}
static constexpr auto scroll(int dx) {
  return [=] { g_displ_y += dx; };
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::yakki;

    load_atlas();
    load_enemies();

    handle(KEY_DOWN, K_UP, scroll(1));
    handle(KEY_DOWN, K_DOWN, scroll(-1));
    handle(KEY_DOWN, K_LEFT, strafe(-1));
    handle(KEY_DOWN, K_RIGHT, strafe(1));

    on_start = [](auto * r) {
      atlas::setup(r);
      atlas::parallax(g_displ_y, {});

      ships::on_update = tick;
      ships::setup(r, enemies::max_enemies);
    };
    on_frame = [](auto * r) {
      atlas::run(r);
      ships::run(r);
    };

    start();
  }
} i;
