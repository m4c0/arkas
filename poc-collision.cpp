#pragma leco app

import dotz;
import quack;
import ships;

static constexpr const dotz::vec2 enemy_pos { -2, -4 };
static dotz::vec2 g_player_pos { 2, 4 };

static void setup_buffer() {
  ships::blit(enemy_pos, { 0, 2 });
  ships::blit(g_player_pos, { 0, 1 });
}

struct init {
  init() {
    using namespace quack::yakki;
    on_start = [](auto * r) {
      ships::on_update = setup_buffer;
      ships::setup(r, 100);
    };
    on_frame = [](auto * r) { ships::run(r); };
    start();
  }
} i;
