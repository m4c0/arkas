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

static auto & at(dotz::ivec2 p) { return g_buffer[p.y][p.x]; }

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
  for (dotz::ivec2 p {}; p.y < plane_h; p.y++) {
    for (p.x = 0; p.x < plane_w; p.x++) {
      blit(i, p * 2, at(p));

      constexpr const dotz::ivec2 dr { 1, 0 };
      constexpr const dotz::ivec2 db { 0, 1 };
      if (p.x < plane_w - 1) {
        auto l = at(p);
        auto r = at(p + dr);
        if (l == r) blit(i, p * 2 + dr, l);
        else blit(i, p * 2 + dr, l + dr);
      }
      if (p.y < plane_h - 1) {
        auto t = at(p);
        auto b = at(p + db);
        if (t == b) blit(i, p * 2 + db, t);
        else blit(i, p * 2 + db, t + db);
      }
      if (p.x < plane_w - 1 && p.y < plane_h - 1) {
        auto tl = at(p);
        auto tr = at(p + dr);
        auto bl = at(p + db);
        auto br = at(p + 1);
        auto pp = p * 2 + 1;
        if (tl == tr && tl == bl && tl == br) blit(i, pp, tl);
        else if (tl == tr && bl == br) blit(i, pp, tl + db);
        else if (tl == bl && tr == br) blit(i, pp, tl + dr);
        else if (tl == br && tr == bl) (void)0;
        else if (tl == tr && tl == bl) blit(i, pp, br - dr - db);
        else if (tl == tr && tl == br) blit(i, pp, bl + dr - db);
        else if (tl == bl && tl == br) blit(i, pp, tr - dr + db);
        else blit(i, pp, tl + dr + db);
      }
    }
  }
  *i++ = {
    .position = g_cursor * 2.f - 0.1f,
    .size = { 1.2f },
    .colour = { 1, 0, 0, 1 },
  };
  blit(i, g_cursor * 2.f, g_brush);
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
    g_brush.x = 1 + 3 * n;
    g_brush.y = 2;
    update_data();
  };
}

static void stamp() {
  at(g_cursor) = g_brush;
  update_data();
}

static void fill(int x, int y, dotz::ivec2 st) {
  if (x < 0 || x >= plane_w || y < 0 || y >= plane_h) return;

  auto & p = g_buffer[y][x];
  if (p != st) return;

  p = g_brush;
  fill(x - 1, y, st);
  fill(x + 1, y, st);
  fill(x, y - 1, st);
  fill(x, y + 1, st);
}
static void fill() {
  auto p = at(g_cursor);
  if (p == g_brush) return;

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

    handle(KEY_DOWN, K_SPACE, stamp);
    handle(KEY_DOWN, K_L, fill);
  }
} i;
