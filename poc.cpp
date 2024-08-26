#pragma leco app
#pragma leco add_resource "atlas.png"

import dotz;
import hai;
import input;
import plane;
import quack;
import rng;
import ships;
import sitime;
import voo;

static quack::yakki::buffer * g_gnd_plane_buffer;
static quack::yakki::buffer * g_sky_plane_buffer;
static quack::yakki::image * g_plane_image;

static constexpr const quack::upc game_area { {}, { 16 } };

struct bullet {
  dotz::vec2 pos {};
  bool active {};
};
static constexpr const auto max_bullets = 16;
static hai::array<bullet> g_bullets { max_bullets };
static float g_gun_cooldown = 1e20;

struct enemy {
  float spawn_disp_y {};
  dotz::vec2 pos {};
  dotz::vec2 speed {};
  dotz::vec2 accel {};
  bool active {};
};
static constexpr const auto max_enemies = 1024;
static hai::array<enemy> g_enemies { max_enemies };

static dotz::vec2 player_pos { -0.5f, 3.5f };
static sitime::stopwatch timer {};

static constexpr const float initial_displ_y = plane::t::draw_h - 2;
static float g_displ_y = initial_displ_y;

static plane::t g_gnd_plane {};
static plane::t g_sky_plane {};

static void update_data() {
  for (auto & e : g_enemies) {
    if (!e.active) continue;

    ships::blit(e.pos, { 0, 2 });
  }

  for (auto & b : g_bullets) {
    if (!b.active) continue;

    ships::blit(b.pos, { 1, 0 });
  }

  ships::blit(player_pos, { 0, 1 });
}

static void move_player(float dt) {
  auto d = input::left_stick();
  if (dotz::length(d) < 0.001) return;

  player_pos = dotz::clamp(player_pos + d * dt * 10.0, { -8 }, { 7 });
}

static void shoot(float dt) {
  g_gun_cooldown += dt;
  if (!input::fire()) return;
  if (g_gun_cooldown < 0.3) return;

  for (auto & b : g_bullets) {
    if (b.active) continue;

    g_gun_cooldown = 0;
    b = { .pos = player_pos, .active = true };
    break;
  }
}

static void move_bullets(float dt) {
  for (auto & b : g_bullets) {
    if (!b.active) continue;

    b.pos.y -= dt * 20.f;
    if (b.pos.y < -1 - game_area.grid_size.y / 2) b.active = false;
  }
}

static void parallax(float dt) {
  constexpr const auto hpw = plane::t::draw_w / 2.f;

  // TODO: fix upper bound
  // TODO: trigger "end level" when upper bound is reached
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

static void move_enemies(float dt) {
  for (auto & e : g_enemies) {
    if (e.spawn_disp_y == 0) continue;

    if (e.active) {
      e.pos = e.pos + e.speed * dt;
      e.speed = e.speed + e.accel * dt;
    } else if (initial_displ_y - e.spawn_disp_y > g_displ_y) {
      e.active = true;
    }
  }
}

static void check_bullet_enemy_collisions() {
  for (auto & b : g_bullets) {
    if (!b.active) continue;

    auto bs = b.pos;
    auto be = bs + 1;

    for (auto & e : g_enemies) {
      if (!e.active) continue;

      auto es = e.pos;
      auto ee = es + 1;

      if (ee.x < bs.x || es.x > be.x) continue;
      if (ee.y < bs.y || es.y > be.y) continue;

      e = {};
      b = {};
      break;
    }
  }
}

static void repaint() {
  float dt = timer.millis() / 1000.f;
  timer = {};

  move_player(dt);
  shoot(dt);
  move_bullets(dt);
  move_enemies(dt);
  check_bullet_enemy_collisions();
  parallax(dt);
  update_data();
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

static void init_enemies() {
  constexpr const auto sy = game_area.grid_size.y;

  auto * e = g_enemies.begin();

  *e++ = { .spawn_disp_y = 2, .pos = { 0.f, -sy }, .speed = { 0, 5 } };
  *e++ = { .spawn_disp_y = 2.5f, .pos = { 0.f, -sy }, .speed = { 0, 5 } };
  *e++ = { .spawn_disp_y = 3, .pos = { 0.f, -sy }, .speed = { 0, 5 } };
  *e++ = { .spawn_disp_y = 3.5f, .pos = { 0.f, -sy }, .speed = { 0, 5 } };

  *e++ = { .spawn_disp_y = 5.0f, .pos = { -5.f, -sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };
  *e++ = { .spawn_disp_y = 5.5f, .pos = { -5.f, -sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };
  *e++ = { .spawn_disp_y = 6.0f, .pos = { -5.f, -sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };
  *e++ = { .spawn_disp_y = 6.5f, .pos = { -5.f, -sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };

  *e++ = { .spawn_disp_y = 8.0f, .pos = { 5.f, -sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
  *e++ = { .spawn_disp_y = 8.5f, .pos = { 5.f, -sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
  *e++ = { .spawn_disp_y = 9.0f, .pos = { 5.f, -sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
  *e++ = { .spawn_disp_y = 9.5f, .pos = { 5.f, -sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
}

struct init {
  init() {
    init_ground_plane();
    init_sky_plane();
    init_enemies();

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
        .grid_pos = { hpw, initial_displ_y - hpw + 2 },
        .grid_size = { plane::t::draw_w - 4 },
      };
      g_gnd_plane_buffer->scissor() = s;

      g_sky_plane_buffer = r->buffer(plane::t::tiles, [](auto *& i) { plane::render(&g_sky_plane, i); });
      g_sky_plane_buffer->pc() = {
        .grid_pos = { hpw, initial_displ_y - hpw + 8 },
        .grid_size = { plane::t::draw_w - 16 },
      };
      g_sky_plane_buffer->scissor() = s;

      ships::on_update = repaint;
      ships::setup(r, max_bullets + max_enemies + 1);

      g_plane_image = r->image("atlas.png");
    };
    on_frame = [](renderer * r) {
      r->run(g_gnd_plane_buffer, g_plane_image);
      r->run(g_sky_plane_buffer, g_plane_image);
      ships::run(r);
    };
    start();
  }
} i;
