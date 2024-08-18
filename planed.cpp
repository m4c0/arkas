#pragma leco app
#pragma leco add_resource "atlas.png"

import casein;
import dotz;
import quack;

enum brush_type { bt_void = 0, bt_water, bt_grass, bt_dirt, bt_cloud };

static constexpr const auto plane_w = 16;
static constexpr const auto plane_h = 32;

static brush_type g_buffer[plane_h][plane_w] {};
static brush_type g_brush {};

static dotz::ivec2 g_cursor {};

static auto & at(dotz::ivec2 p) { return g_buffer[p.y][p.x]; }

static constexpr dotz::ivec2 uv0(brush_type a, brush_type b) {
  switch (a) {
    case bt_void:
      if (b == bt_void) return { 1, 5 };
      if (b == bt_cloud) return { 1, 5 };
      break;
    case bt_water:
      if (b == bt_water) return { 1, 2 };
      if (b == bt_grass) return { 1, 2 };
      break;
    case bt_grass:
      if (b == bt_grass) return { 4, 2 };
      if (b == bt_water) return { 4, 2 };
      if (b == bt_dirt) return { 7, 2 };
      break;
    case bt_dirt:
      if (b == bt_dirt) return { 10, 2 };
      if (b == bt_grass) return { 10, 2 };
      break;
    case bt_cloud:
      if (b == bt_cloud) return { 4, 5 };
      if (b == bt_void) return { 4, 5 };
      break;
    default: break;
  }
  return {};
}
static constexpr dotz::ivec2 uv0(brush_type a, brush_type b, dotz::ivec2 d) {
  auto p = uv0(a, b);
  return p == 0 ? 0 : p + d;
}
static constexpr dotz::ivec2 uv0(brush_type a) { return uv0(a, a); }

static void blit(quack::instance *& i, dotz::vec2 p, dotz::ivec2 uv0) {
  if (uv0 == 0) return;

  *i++ = {
    .position = p,
    .size = { 1 },
    .uv0 = uv0 / 16.f,
    .uv1 = (uv0 + 1) / 16.f,
    .multiplier = { 1 },
  };
}

static void update_data(quack::instance *& i) {
  for (dotz::ivec2 p {}; p.y < plane_h; p.y++) {
    for (p.x = 0; p.x < plane_w; p.x++) {
      blit(i, p * 2, uv0(at(p)));

      constexpr const dotz::ivec2 dr { 1, 0 };
      constexpr const dotz::ivec2 db { 0, 1 };
      if (p.x < plane_w - 1) {
        auto l = at(p);
        auto r = at(p + dr);
        if (l == r) blit(i, p * 2 + dr, uv0(l, r));
        else blit(i, p * 2 + dr, uv0(l, r, dr));
      }
      if (p.y < plane_h - 1) {
        auto t = at(p);
        auto b = at(p + db);
        if (t == b) blit(i, p * 2 + db, uv0(t, b));
        else blit(i, p * 2 + db, uv0(t, b, db));
      }
      if (p.x < plane_w - 1 && p.y < plane_h - 1) {
        auto tl = at(p);
        auto tr = at(p + dr);
        auto bl = at(p + db);
        auto br = at(p + 1);
        auto pp = p * 2 + 1;
        if (tl == tr && tl == bl && tl == br) blit(i, pp, uv0(tl));
        else if (tl == tr && bl == br) blit(i, pp, uv0(tl, bl, db));
        else if (tl == bl && tr == br) blit(i, pp, uv0(tl, tr, dr));
        else if (tl == br && tr == bl) blit(i, pp, 0);
        else if (tl == tr && tl == bl) blit(i, pp, uv0(br, tl, -dr - db));
        else if (tl == tr && tl == br) blit(i, pp, uv0(bl, tl, dr - db));
        else if (tl == bl && tl == br) blit(i, pp, uv0(tr, tl, -dr + db));
        else if (br == bl && br == tr) blit(i, pp, uv0(tl, br, dr + db));
      }
    }
  }
  *i++ = {
    .position = g_cursor * 2.f - 0.1f,
    .size = { 1.2f },
    .colour = { 1, 0, 0, 1 },
  };
  blit(i, g_cursor * 2.f, uv0(g_brush));
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
static constexpr auto brush(brush_type n) {
  return [=] {
    g_brush = n;
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

    handle(KEY_DOWN, K_0, brush(bt_void));
    handle(KEY_DOWN, K_1, brush(bt_water));
    handle(KEY_DOWN, K_2, brush(bt_grass));
    handle(KEY_DOWN, K_3, brush(bt_dirt));
    handle(KEY_DOWN, K_9, brush(bt_cloud));

    handle(KEY_DOWN, K_SPACE, stamp);
    handle(KEY_DOWN, K_L, fill);
  }
} i;
