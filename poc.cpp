#pragma leco app
#pragma leco add_resource "atlas.png"

import dotz;
import input;
import plane;
import quack;
import sitime;
import voo;

static dotz::vec2 player_pos {};
static sitime::stopwatch timer {};

static plane::t pl {};

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

static void init_plane() {
  for (auto y = 0; y < plane::t::h; y++) {
    for (auto x = 0; x < plane::t::w; x++) {
      pl.at({ x, y }) = plane::at_water;
    }
  }

  const auto r = plane::t::w - 2;
  const auto b = plane::t::h - 2;

  for (auto y = 1; y <= b; y++) {
    pl.at({ 1, y }) = plane::at_grass;
    pl.at({ r, y }) = plane::at_grass;
  }
  for (auto x = 1; x <= r; x++) {
    pl.at({ x, 1 }) = plane::at_grass;
    pl.at({ x, b }) = plane::at_grass;
  }
}

static quack::yakki::buffer * g_plane_buffer;
static quack::yakki::buffer * g_top_buffer;
static quack::yakki::image * g_image;

struct init {
  init() {
    init_plane();

    input::setup_defaults();

    using namespace quack::yakki;
    on_start = [](resources * r) {
      auto hpw = plane::t::draw_w / 2.f;

      g_plane_buffer = r->buffer(plane::t::tiles, [](auto *& i) { plane::render(&pl, i); });
      g_plane_buffer->pc() = {
        .grid_pos = { hpw, plane::t::draw_h - hpw },
        .grid_size = { plane::t::draw_w },
      };

      g_top_buffer = r->buffer(1, &repaint);
      g_top_buffer->pc() = {
        .grid_pos = { 0, -6 },
        .grid_size = { 16, 16 },
      };
      g_top_buffer->start();

      g_image = r->image("atlas.png");
    };
    on_frame = [](renderer * r) {
      r->run(g_plane_buffer, g_image);
      r->run(g_top_buffer, g_image);
    };
  }
} i;
