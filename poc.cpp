#pragma leco app
#pragma leco add_resource "atlas.png"

import dotz;
import input;
import quack;
import sitime;
import voo;

static dotz::vec2 player_pos {};
static sitime::stopwatch timer {};

static void update_data(quack::instance *& i) {
  *i++ = {
    .position = player_pos,
    .size = { 1, 1 },
    .colour = { 1, 0, 1, 1 },
  };
}

static void move_player(float dt) {
  auto d = input::left_stick();
  if (dotz::length(d) < 0.001) return;

  player_pos = player_pos + d * dt * 10.0;
}

static void repaint(quack::instance *& i) {
  float dt = timer.millis() / 1000.f;
  timer = {};

  move_player(dt);
  update_data(i);
}

static quack::yakki::buffer * g_buffer;
static quack::yakki::image * g_image;

struct init {
  init() {
    input::setup_defaults();

    using namespace quack::yakki;
    on_start = [](resources * r) {
      g_buffer = r->buffer(1024, &repaint);
      g_buffer->pc() = {
        .grid_pos = { 0, -6 },
        .grid_size = { 16, 16 },
      };
      g_buffer->start();

      g_image = r->image("atlas.png");
    };
    on_frame = [](renderer * r) { r->run(g_buffer, g_image); };
  }
} i;
