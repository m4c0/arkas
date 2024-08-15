#pragma leco app

import casein;
import dotz;
import input;
import quack;
import sitime;

static dotz::vec2 player_pos {};
static sitime::stopwatch timer {};

static void update_data() {
  quack::donald::data([](auto *& i) {
    *i++ = {
      .position = player_pos,
      .size = { 1, 1 },
      .colour = { 1, 0, 0, 1 },
    };
  });
}

static void move_player(float dt) {
  auto d = input::left_stick();
  if (dotz::length(d) < 0.001) return;

  player_pos = player_pos + d * dt * 10.0;
}

static void repaint() {
  float dt = timer.millis() / 1000.f;
  timer = {};

  move_player(dt);
  update_data();
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::donald;

    app_name("arkas");
    max_quads(1024);

    clear_colour({ 0, 0, 0, 1 });

    push_constants({
        .grid_pos = { 0, -6 },
        .grid_size = { 16, 16 },
    });

    input::setup_defaults();

    repaint();
    handle(REPAINT, repaint);
  }
} i;
