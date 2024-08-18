#pragma leco app
#pragma leco add_resource "atlas.png"

import casein;
import dotz;
import quack;

enum brush_type { bt_void = 0, bt_water, bt_grass, bt_dirt, bt_cloud };

static constexpr const auto plane_w = 16;
static constexpr const auto plane_h = 32;

struct plane {
  brush_type ane[plane_h][plane_w] {};
  dotz::ivec2 cursor {};
};

static plane g_plane_t {};
static plane g_plane_b {};
static brush_type g_brush {};

static auto * plane_of(brush_type b) {
  switch (b) {
    case bt_void:
    case bt_cloud: return &g_plane_t;
    default: return &g_plane_b;
  }
}

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

static void update_plane_data(quack::instance *& i, const plane & pl) {
  const auto at = [&](dotz::ivec2 p) { return pl.ane[p.y][p.x]; };

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
        else if (tl == br && tr == bl) continue;
        else if (tl == tr && tl == bl) blit(i, pp, uv0(br, tl, -dr - db));
        else if (tl == tr && tl == br) blit(i, pp, uv0(bl, tl, dr - db));
        else if (tl == bl && tl == br) blit(i, pp, uv0(tr, tl, -dr + db));
        else if (br == bl && br == tr) blit(i, pp, uv0(tl, br, dr + db));
      }
    }
  }
}
static void update_data(quack::instance *& i) {
  bool cursor_on_plane_b = plane_of(g_brush) == &g_plane_b;

  update_plane_data(i, g_plane_b);
  if (!cursor_on_plane_b) update_plane_data(i, g_plane_t);

  auto cur = cursor_on_plane_b ? g_plane_b.cursor : g_plane_t.cursor;
  *i++ = {
    .position = cur * 2.f - 0.1f,
    .size = { 1.2f },
    .colour = { 1, 0, 0, 1 },
  };
  blit(i, cur * 2.f, uv0(g_brush));

  constexpr const auto hpw = plane_w / 2;
  auto y = dotz::clamp(cur.y, hpw, plane_h - hpw);
  quack::donald::push_constants({
      .grid_pos = dotz::vec2 { hpw, y } * 2.f + 0.5,
      .grid_size = dotz::vec2 { plane_w } * 2.f + 1.f,
  });
}
static void update_data() { quack::donald::data(::update_data); }

static constexpr auto move(int dx, int dy) {
  return [=] {
    auto * pl = plane_of(g_brush);
    auto c = pl->cursor + dotz::ivec2 { dx, dy };
    pl->cursor = dotz::clamp(c, { 0 }, { plane_w - 1, plane_h - 1 });
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
  auto * pl = plane_of(g_brush);
  pl->ane[pl->cursor.y][pl->cursor.x] = g_brush;
  update_data();
}

static void fill(int x, int y, dotz::ivec2 st, plane * pl) {
  if (x < 0 || x >= plane_w || y < 0 || y >= plane_h) return;

  auto & p = pl->ane[y][x];
  if (p != st) return;

  p = g_brush;
  fill(x - 1, y, st, pl);
  fill(x + 1, y, st, pl);
  fill(x, y - 1, st, pl);
  fill(x, y + 1, st, pl);
}
static void fill() {
  auto pl = plane_of(g_brush);
  auto p = pl->ane[pl->cursor.y][pl->cursor.x];
  if (p == g_brush) return;

  fill(pl->cursor.x, pl->cursor.y, p, pl);
  update_data();
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::donald;

    clear_colour({ 0.0f, 0.0f, 0.0f, 1.f });
    max_quads(102400);

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
