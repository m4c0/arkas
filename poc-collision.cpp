#pragma leco app

import casein;
import collision;
import dotz;
import pixed;
import quack;
import ships;
import silog;

static constexpr const dotz::vec2 enemy_pos { -2, -4 };
static dotz::vec2 g_player_pos { 2, 4 };
static bool g_blink {};

static void setup_buffer() {
  if (g_blink && collision::between(g_player_pos, { 0, 1 }, enemy_pos, { 0, 2 })) return;

  ships::blit(enemy_pos, { 0, 2 });
  ships::blit(g_player_pos, { 0, 1 });
}

static void mouse_move() { g_player_pos = ships::mouse_pos() - 0.5f; }

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

    collision::setup();

    start();
  }
} i;
