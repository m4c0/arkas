#pragma leco app

import casein;
import dotz;
import pixed;
import quack;
import ships;
import silog;

static constexpr const dotz::vec2 enemy_pos { -2, -4 };
static dotz::vec2 g_player_pos { 2, 4 };
static bool g_blink {};

static bool collided() {
  auto bs = g_player_pos;
  auto be = bs + 1;

  auto es = enemy_pos;
  auto ee = es + 1;

  if (ee.x < bs.x || es.x > be.x) return false;
  if (ee.y < bs.y || es.y > be.y) return false;

  return true;
}

static void setup_buffer() {
  if (g_blink && collided()) return;

  ships::blit(enemy_pos, { 0, 2 });
  ships::blit(g_player_pos, { 0, 1 });
}

static void mouse_move() { g_player_pos = ships::mouse_pos() - 0.5f; }

struct mask {
  unsigned rows[16] {};
};
static mask g_masks[16][16] {};
static void create_bitmaps(const pixed::context & ctx) {
  for (auto y = 0, p = 0; y < ctx.h; y++) {
    for (auto x = 0; x < ctx.w; x++, p++) {
      auto sx = x / 16;
      auto sy = y / 16;
      auto px = x % 16;
      auto py = y % 16;

      if (ctx.image[p].a < 128) continue;

      auto & row = g_masks[sy][sx].rows[py];
      row |= 1 << (15 - px);
    }
  }

  for (auto r : g_masks[2][0].rows) {
    silog::log(silog::debug, "%08o", r);
  }
  for (auto r : g_masks[1][0].rows) {
    silog::log(silog::debug, "%08o", r);
  }
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::yakki;

    handle(MOUSE_MOVE, mouse_move);
    handle(TIMER, [] { g_blink = !g_blink; });

    on_start = [](auto * r) {
      ships::on_update = setup_buffer;
      ships::setup(r, 100);
    };
    on_frame = [](auto * r) { ships::run(r); };

    pixed::read("ships.png")
        .map([](auto & ctx) {
          create_bitmaps(ctx);
          start();
        })
        .log_error();
  }
} i;
