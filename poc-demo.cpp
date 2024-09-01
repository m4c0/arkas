#pragma leco app

import atlas;
import casein;
import enemies;
import level;
import plane;
import quack;
import rng;
import ships;

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

    level::load();

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
