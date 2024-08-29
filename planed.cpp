#pragma leco app
#pragma leco add_resource "atlas.png"

import atlas;
import casein;
import dotz;
import plane;
import quack;

static quack::yakki::buffer * g_buffer {};

static dotz::ivec2 g_cursor {};
static plane::area_type g_brush {};

static void update_data(quack::instance *& i) {
  *i++ = {
    .position = g_cursor * 2.f - 0.1f,
    .size = { 1.2f },
    .colour = { 1, 0, 0, 1 },
  };
  plane::blit(i, g_cursor * 2.f, g_brush);
}

static void update_ui() {
  atlas::parallax(atlas::initial_displ_y - g_cursor.y, { 0.f, 0.f });

  g_buffer->pc() = atlas::ground_pc();
  g_buffer->run_once();
}
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

static void stamp() { atlas::ground(g_cursor) = g_brush; }

static void fill(int x, int y, dotz::ivec2 st) {
  if (x < 0 || x >= plane::t::w || y < 0 || y >= plane::t::h) return;

  auto & p = atlas::ground({ x, y });
  if (p != st) return;

  p = g_brush;
  fill(x - 1, y, st);
  fill(x + 1, y, st);
  fill(x, y - 1, st);
  fill(x, y + 1, st);
}
static void fill() {
  auto p = atlas::ground(g_cursor);
  if (p == g_brush) return;

  fill(g_cursor.x, g_cursor.y, p);
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::yakki;

    for (auto y = 0; y < plane::t::h; y++) {
      for (auto x = 0; x < plane::t::w; x++) {
        atlas::ground({ x, y }) = plane::at_water;
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
      atlas::setup(r);
      g_buffer = r->buffer(2, update_data);
      update_ui();
    };
    on_frame = [](auto * r) {
      atlas::run(r);
      r->run(g_buffer, atlas::plane_image());
    };

    start();
  }
} i;
