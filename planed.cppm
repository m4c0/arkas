#pragma leco app
#pragma leco add_resource "atlas.png"
export module planed;
import :demo;
import atlas;
import casein;
import dotz;
import plane;
import quack;

static quack::yakki::buffer * g_atlas_buffer {};
static quack::yakki::buffer * g_ui_buffer {};
static dotz::ivec2 g_cursor {};
static plane::area_type g_brush {};

static void update_data(quack::instance *& i) {
  *i++ = {
    .position = g_cursor * 2.f - 0.1f,
    .size = { 1.2f },
    .colour = { 1, 0, 0, 1 },
  };
  plane::blit(i, g_cursor * 2.f, plane::uv0(g_brush));
}

static void update_atlas() { g_atlas_buffer->run_once(); }

static void update_ui() {
  constexpr const auto w = plane::t::draw_w;
  constexpr const auto hpw = w / 2.0f;

  auto y = dotz::min(dotz::max(hpw, g_cursor.y * 2.0f), plane::t::draw_h - hpw);

  g_atlas_buffer->pc() = { { hpw, y }, { w } };
  g_ui_buffer->pc() = { { hpw, y }, { w } };
  g_ui_buffer->run_once();
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

    switch (n) {
      case plane::at_cloud:
      case plane::at_void: g_atlas_buffer = atlas::sky(); break;
      default: g_atlas_buffer = atlas::ground(); break;
    }

    update_ui();
  };
}

static void stamp() {
  if (g_atlas_buffer == atlas::ground()) {
    atlas::ground(g_cursor) = g_brush;
  } else {
    atlas::sky(g_cursor) = g_brush;
  }

  update_atlas();
}

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
  update_atlas();
}

namespace planed::editor {
  void start() {
    using namespace casein;
    using namespace quack::yakki;

    reset_k(KEY_DOWN);
    reset_k(KEY_UP);

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

    handle(KEY_DOWN, K_ESCAPE, planed::demo::start);

    on_frame = [](auto * r) {
      r->run(g_atlas_buffer, atlas::image());
      r->run(g_ui_buffer, atlas::image());
    };
  }
} // namespace planed::editor

struct init {
  init() {
    using namespace casein;
    using namespace quack::yakki;

    on_start = [](auto * r) {
      atlas::setup(r);
      planed::demo::setup(r);

      g_atlas_buffer = atlas::ground();
      g_ui_buffer = r->buffer(2, update_data);
      update_ui();
    };

    planed::editor::start();

    start();
  }
} i;
