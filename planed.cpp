#pragma leco app
#pragma leco add_resource "atlas.png"

import casein;
import dotz;
import quack;

static constexpr const auto plane_w = 16;
static constexpr const auto plane_h = 32;

static dotz::ivec2 g_buffer[plane_h][plane_w] {};
static dotz::ivec2 g_cursor {};
static dotz::ivec2 g_brush { 1, 2 };
static dotz::ivec2 g_brush_d = g_brush;

static void blit(quack::instance *& i, dotz::vec2 p, dotz::ivec2 brush) {
  *i++ = {
    .position = p,
    .size = { 1 },
    .uv0 = brush / 16.f,
    .uv1 = (brush + 1) / 16.f,
    .multiplier = { 1 },
  };
}

static void update_data(quack::instance *& i) {
  for (auto y = 0; y < plane_h; y++) {
    for (auto x = 0; x < plane_w; x++) {
      blit(i, dotz::vec2 { x, y } * 2.f, g_buffer[y][x]);
    }
  }
  for (auto y = 0; y < plane_h; y++) {
    for (auto x = 0; x < plane_w - 1; x++) {
      auto l = g_buffer[y][x];
      auto r = g_buffer[y][x + 1];
      if (l == r) blit(i, dotz::vec2 { x * 2 + 1, y * 2 }, l);
    }
  }
  *i++ = {
    .position = g_cursor * 2.f - 0.1f,
    .size = { 1.2f },
    .colour = { 1, 0, 0, 1 },
  };
  blit(i, g_cursor * 2.f, g_brush_d);
}
static void update_data() {
  using namespace quack::donald;

  constexpr const auto hpw = plane_w / 2;
  auto y = dotz::clamp(g_cursor.y, hpw, plane_h - hpw);
  push_constants({
      .grid_pos = dotz::vec2 { hpw, y } * 2.f + 0.5,
      .grid_size = dotz::vec2 { plane_w } * 2.f + 1.f,
  });
  data(::update_data);
}

static constexpr auto move(int dx, int dy) {
  return [=] {
    auto c = g_cursor + dotz::ivec2 { dx, dy };
    g_cursor = dotz::clamp(c, { 0 }, { plane_w - 1, plane_h - 1 });
    update_data();
  };
}
static constexpr auto brush(unsigned n) {
  return [=] {
    auto d = g_brush_d - g_brush;
    g_brush.x = 1 + 3 * n;
    g_brush.y = 2;
    g_brush_d = g_brush + d;
    update_data();
  };
}
static constexpr auto brush_d(int dx, int dy) {
  return [=] {
    g_brush_d = g_brush + dotz::ivec2 { dx, dy };
    update_data();
  };
}

static void stamp() {
  g_buffer[g_cursor.y][g_cursor.x] = g_brush_d;
  update_data();
}

static void fill(int x, int y, dotz::ivec2 st) {
  if (x < 0 || x >= plane_w || y < 0 || y >= plane_h) return;

  auto & p = g_buffer[y][x];
  if (p != st) return;

  p = g_brush_d;
  fill(x - 1, y, st);
  fill(x + 1, y, st);
  fill(x, y - 1, st);
  fill(x, y + 1, st);
}
static void fill() {
  auto p = g_buffer[g_cursor.y][g_cursor.x];
  if (p == g_brush_d) return;

  fill(g_cursor.x, g_cursor.y, p);
  update_data();
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::donald;

    clear_colour({ 0.0f, 0.0f, 0.0f, 1.f });
    max_quads(10240);

    atlas("atlas.png");
    update_data();

    handle(KEY_DOWN, K_UP, move(0, -1));
    handle(KEY_DOWN, K_DOWN, move(0, 1));
    handle(KEY_DOWN, K_LEFT, move(-1, 0));
    handle(KEY_DOWN, K_RIGHT, move(1, 0));

    handle(KEY_DOWN, K_1, brush(0));
    handle(KEY_DOWN, K_2, brush(1));

    handle(KEY_DOWN, K_Q, brush_d(-1, -1));
    handle(KEY_DOWN, K_W, brush_d(+0, -1));
    handle(KEY_DOWN, K_E, brush_d(+1, -1));
    handle(KEY_DOWN, K_A, brush_d(-1, +0));
    handle(KEY_DOWN, K_S, brush_d(+0, +0));
    handle(KEY_DOWN, K_D, brush_d(+1, +0));
    handle(KEY_DOWN, K_Z, brush_d(-1, +1));
    handle(KEY_DOWN, K_X, brush_d(+0, +1));
    handle(KEY_DOWN, K_C, brush_d(+1, +1));

    handle(KEY_DOWN, K_SPACE, stamp);
    handle(KEY_DOWN, K_L, fill);
  }
} i;
