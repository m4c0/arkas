#pragma leco app

import atlas;
import casein;
import plane;
import quack;
import rng;

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

static float g_x = 0;
static float g_displ_y = atlas::initial_displ_y;

static constexpr auto strafe(int dx) {
  return [=] {
    g_x += dx;
    atlas::parallax(g_displ_y, { g_x, 0.f });
  };
}
static constexpr auto scroll(int dx) {
  return [=] {
    g_displ_y += dx;
    atlas::parallax(g_displ_y, { g_x, 0.f });
  };
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::yakki;

    load_atlas();

    handle(KEY_DOWN, K_UP, scroll(-1));
    handle(KEY_DOWN, K_DOWN, scroll(1));
    handle(KEY_DOWN, K_LEFT, strafe(-1));
    handle(KEY_DOWN, K_RIGHT, strafe(1));

    on_start = [](auto * r) {
      atlas::setup(r);
      atlas::parallax(g_displ_y, {});
    };
    on_frame = [](auto * r) { atlas::run(r); };

    start();
  }
} i;
