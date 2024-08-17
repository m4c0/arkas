#pragma leco app
#pragma leco add_resource "atlas.png"

import casein;
import dotz;
import quack;

static dotz::ivec2 g_cursor{};
static dotz::ivec2 g_brush{ 1, 2 };

static void update_data(quack::instance *& i) {
  *i++ = {
    .position = g_cursor - 0.1f,
    .size = { 1.2f },
    .colour = { 1, 0, 0, 1 },
  };
  *i++ = {
    .position = g_cursor,
    .size = { 1 },
    .uv0 = (g_brush - 1) / 16.f,
    .uv1 = (g_brush + 2) / 16.f,
    .multiplier = { 1 },
  };
}
static void update_data() { quack::donald::data(::update_data); }

static constexpr auto move(int dx, int dy) {
  return [=] {
    auto c = g_cursor + dotz::ivec2 { dx, dy };
    g_cursor = dotz::clamp(c, { 0 }, { 31 });
    update_data();
  };
}
static constexpr auto brush(unsigned n) {
  return [=] {
    g_brush.x = 1 + 3 * n;
    g_brush.y = 2;
    update_data();
  };
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::donald;

    clear_colour({ 0.0f, 0.0f, 0.0f, 1.f });
    push_constants({
        .grid_pos = { 16, 16 },
        .grid_size = { 32, 32 },
    });
    max_quads(1024);

    atlas("atlas.png");
    update_data();

    handle(KEY_DOWN, K_UP, move(0, -1));
    handle(KEY_DOWN, K_DOWN, move(0, 1));
    handle(KEY_DOWN, K_LEFT, move(-1, 0));
    handle(KEY_DOWN, K_RIGHT, move(1, 0));

    handle(KEY_DOWN, K_1, brush(0));
    handle(KEY_DOWN, K_2, brush(1));
  }
} i;
