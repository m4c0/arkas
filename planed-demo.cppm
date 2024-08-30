export module planed:demo;

import atlas;
import casein;
import enemies;
import plane;
import quack;
import ships;

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
  enemies::blit();
}

static void update_parallax() { atlas::parallax(atlas::initial_displ_y - g_displ_y, { g_x, 0.f }); }

static constexpr auto strafe(int dx) {
  return [=] {
    g_x += dx;
    update_parallax();
  };
}
static constexpr auto scroll(int dx) {
  return [=] {
    g_displ_y += dx;
    update_parallax();
  };
}

namespace planed::editor {
  void start();
}

namespace planed::demo {
  void setup(quack::yakki::resources * r) {
    ships::on_update = tick;
    ships::setup(r, enemies::max_enemies);
  }

  void start() {
    using namespace casein;
    using namespace quack::yakki;

    load_enemies();

    reset_k(KEY_DOWN);

    handle(KEY_DOWN, K_UP, scroll(1));
    handle(KEY_DOWN, K_DOWN, scroll(-1));
    handle(KEY_DOWN, K_LEFT, strafe(-1));
    handle(KEY_DOWN, K_RIGHT, strafe(1));

    handle(KEY_DOWN, K_ENTER, planed::editor::start);

    on_frame = [](auto * r) {
      atlas::run(r);
      ships::run(r);
    };

    update_parallax();
  }
} // namespace planed::demo
