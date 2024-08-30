#pragma leco app
#pragma leco add_resource "atlas.png"

import casein;
import dotz;
import plane;
import quack;

static quack::yakki::buffer * g_buffer {};
static quack::yakki::image * g_image {};
static plane::t g_plane {};

static dotz::ivec2 g_cursor {};
static plane::area_type g_brush {};

static void update_data(quack::instance *& i) {
  plane::render(&g_plane, i);

  *i++ = {
    .position = g_cursor * 2.f - 0.1f,
    .size = { 1.2f },
    .colour = { 1, 0, 0, 1 },
  };
  plane::blit(i, g_cursor * 2.f, plane::uv0(g_brush));
}

static void update_ui() { g_buffer->run_once(); }
static constexpr auto move(int dx, int dy) {
  return [=] {
    auto c = g_cursor + dotz::ivec2 { dx, dy };
    g_cursor = dotz::clamp(c, { 0 }, { plane::t::w - 1, plane::t::h - 1 });
    update_ui();
  };
}
static constexpr auto brush(plane::area_type n) {
  return [=] {
    g_brush = n;
    g_buffer->run_once();
  };
}

static void stamp() {
  g_plane.at(g_cursor) = g_brush;
  g_buffer->run_once();
}

static void fill(int x, int y, dotz::ivec2 st) {
  if (x < 0 || x >= plane::t::w || y < 0 || y >= plane::t::h) return;

  auto & p = g_plane.at({ x, y });
  if (p != st) return;

  p = g_brush;
  fill(x - 1, y, st);
  fill(x + 1, y, st);
  fill(x, y - 1, st);
  fill(x, y + 1, st);
}
static void fill() {
  auto p = g_plane.at(g_cursor);
  if (p == g_brush) return;

  fill(g_cursor.x, g_cursor.y, p);
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::yakki;

    for (auto y = 0; y < plane::t::h; y++) {
      for (auto x = 0; x < plane::t::w; x++) {
        g_plane.at({ x, y }) = plane::at_water;
      }
    }

    handle(KEY_DOWN, K_UP, move(0, -1));
    handle(KEY_DOWN, K_DOWN, move(0, 1));
    handle(KEY_DOWN, K_LEFT, move(-1, 0));
    handle(KEY_DOWN, K_RIGHT, move(1, 0));

    handle(KEY_DOWN, K_0, brush(plane::at_void));
    handle(KEY_DOWN, K_1, brush(plane::at_water));
    handle(KEY_DOWN, K_2, brush(plane::at_grass));
    handle(KEY_DOWN, K_3, brush(plane::at_dirt));
    handle(KEY_DOWN, K_9, brush(plane::at_cloud));

    handle(KEY_DOWN, K_SPACE, stamp);
    handle(KEY_DOWN, K_L, fill);

    on_start = [](auto * r) {
      constexpr const auto w = plane::t::draw_w;

      g_buffer = r->buffer(plane::t::tiles + 2, update_data);
      g_buffer->pc() = { { w / 2.f }, { w } };
      g_image = r->image("atlas.png");
      update_ui();
    };
    on_frame = [](auto * r) { r->run(g_buffer, g_image); };

    start();
  }
} i;
