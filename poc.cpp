#pragma leco app
#pragma leco add_resource "atlas.png"

import dotz;
import input;
import plane;
import quack;
import rng;
import sitime;
import voo;

static quack::yakki::buffer * g_gnd_plane_buffer;
static quack::yakki::buffer * g_sky_plane_buffer;
static quack::yakki::buffer * g_top_buffer;
static quack::yakki::image * g_image;

static constexpr const quack::upc game_area { {}, { 16 } };

static dotz::vec2 player_pos { -0.5f, 2.5f };
static sitime::stopwatch timer {};
static float g_displ_y = plane::t::draw_h - 2;

static plane::t g_gnd_plane {};
static plane::t g_sky_plane {};

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

  player_pos = dotz::clamp(player_pos + d * dt * 10.0, { -8 }, { 7 });
}

static void parallax(float dt) {
  constexpr const auto hpw = plane::t::draw_w / 2.f;

  g_displ_y = dotz::max(0, g_displ_y - dt);

  auto plane_dx = 2.0f * (player_pos.x + 0.5f) / 8.0f;
  auto plane_dy = 2.0f * (player_pos.y + 0.5f) / 8.0f;

  g_gnd_plane_buffer->pc().grid_pos = {
    plane_dx + plane::t::draw_w / 2.0f,
    plane_dy + g_displ_y - hpw + 2.f,
  };

  plane_dx = 8.0f * (player_pos.x + 0.5f) / 8.0f;
  g_sky_plane_buffer->pc().grid_pos = {
    plane_dx + plane::t::draw_w / 2.0f,
    plane_dy + g_displ_y - hpw + 8.f,
  };
}

static void repaint(quack::instance *& i) {
  float dt = timer.millis() / 1000.f;
  timer = {};

  move_player(dt);
  parallax(dt);
  update_data(i);
}

static void init_ground_plane() {
  for (auto y = 0; y < plane::t::h; y++) {
    for (auto x = 0; x < plane::t::w; x++) {
      g_gnd_plane.at({ x, y }) = plane::at_water;
    }
  }

  const auto r = plane::t::w - 2;
  const auto b = plane::t::h - 2;

  for (auto y = 1; y <= b; y++) {
    g_gnd_plane.at({ 1, y }) = plane::at_grass;
    g_gnd_plane.at({ r, y }) = plane::at_grass;

    g_gnd_plane.at({ static_cast<int>(rng::rand(r)), y }) = plane::at_grass;
  }
  for (auto x = 1; x <= r; x++) {
    g_gnd_plane.at({ x, 1 }) = plane::at_grass;
    g_gnd_plane.at({ x, b }) = plane::at_grass;
  }
}

static void init_sky_plane() {
  const auto r = plane::t::w - 2;
  const auto b = plane::t::h - 2;

  for (auto y = 1; y <= b; y++) {
    g_sky_plane.at({ 1, y }) = plane::at_cloud;
    g_sky_plane.at({ r, y }) = plane::at_cloud;

    g_sky_plane.at({ static_cast<int>(rng::rand(r)), y }) = plane::at_cloud;
  }
  for (auto x = 1; x <= r; x++) {
    g_sky_plane.at({ x, 1 }) = plane::at_cloud;
    g_sky_plane.at({ x, b }) = plane::at_cloud;
  }
}

struct init {
  init() {
    init_ground_plane();
    init_sky_plane();

    input::setup_defaults();

    using namespace quack::yakki;
    on_start = [](resources * r) {
      auto hpw = plane::t::draw_w / 2.f;

      quack::scissor s {
        .offset = -game_area.grid_size / 2,
        .extent = game_area.grid_size,
        .ref = &game_area,
      };

      g_gnd_plane_buffer = r->buffer(plane::t::tiles, [](auto *& i) { plane::render(&g_gnd_plane, i); });
      g_gnd_plane_buffer->pc() = {
        .grid_pos = { hpw, g_displ_y - hpw + 2 },
        .grid_size = { plane::t::draw_w - 4 },
      };
      g_gnd_plane_buffer->scissor() = s;

      g_sky_plane_buffer = r->buffer(plane::t::tiles, [](auto *& i) { plane::render(&g_sky_plane, i); });
      g_sky_plane_buffer->pc() = {
        .grid_pos = { hpw, g_displ_y - hpw + 8 },
        .grid_size = { plane::t::draw_w - 16 },
      };
      g_sky_plane_buffer->scissor() = s;

      g_top_buffer = r->buffer(1, &repaint);
      g_top_buffer->pc() = game_area;
      g_top_buffer->start();

      g_image = r->image("atlas.png");
    };
    on_frame = [](renderer * r) {
      r->run(g_gnd_plane_buffer, g_image);
      r->run(g_sky_plane_buffer, g_image);
      r->run(g_top_buffer, g_image);
    };
  }
} i;
