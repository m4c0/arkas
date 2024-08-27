#pragma leco app

import atlas;
import collision;
import dotz;
import hai;
import input;
import plane;
import quack;
import rng;
import ships;
import sitime;
import voo;

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

static constexpr const float initial_displ_y = atlas::initial_displ_y;
static float g_displ_y = initial_displ_y;

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
    if (b.pos.y < atlas::min_area_y - 1) b.active = false;
  }
}

static void parallax(float dt) {
  // TODO: fix upper bound
  // TODO: trigger "end level" when upper bound is reached
  g_displ_y = dotz::max(0, g_displ_y - dt);
  atlas::parallax(g_displ_y, player_pos);
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

    for (auto & e : g_enemies) {
      if (!e.active) continue;

      if (!collision::between(e.pos, { 0, 2 }, b.pos, { 1, 0 })) continue;

      e = {};
      b = {};
      break;
    }
  }
}

static void tick() {
  float dt = timer.millis() / 1000.f;
  timer = {};

  check_bullet_enemy_collisions();
  move_player(dt);
  shoot(dt);
  move_bullets(dt);
  move_enemies(dt);
  parallax(dt);
  update_data();
}

static void init_ground_plane() {
  for (auto y = 0; y < plane::t::h; y++) {
    for (auto x = 0; x < plane::t::w; x++) {
      atlas::ground({ x, y }) = plane::at_water;
    }
  }

  const auto r = plane::t::w - 2;
  const auto b = plane::t::h - 2;

  for (auto y = 1; y <= b; y++) {
    atlas::ground({ 1, y }) = plane::at_grass;
    atlas::ground({ r, y }) = plane::at_grass;

    atlas::ground({ static_cast<int>(rng::rand(r)), y }) = plane::at_grass;
  }
  for (auto x = 1; x <= r; x++) {
    atlas::ground({ x, 1 }) = plane::at_grass;
    atlas::ground({ x, b }) = plane::at_grass;
  }
}

static void init_sky_plane() {
  const auto r = plane::t::w - 2;
  const auto b = plane::t::h - 2;

  for (auto y = 1; y <= b; y++) {
    atlas::sky({ 1, y }) = plane::at_cloud;
    atlas::sky({ r, y }) = plane::at_cloud;

    atlas::sky({ static_cast<int>(rng::rand(r)), y }) = plane::at_cloud;
  }
  for (auto x = 1; x <= r; x++) {
    atlas::sky({ x, 1 }) = plane::at_cloud;
    atlas::sky({ x, b }) = plane::at_cloud;
  }
}

static void init_enemies() {
  constexpr const auto sy = atlas::min_area_y - 1;

  auto * e = g_enemies.begin();

  *e++ = { .spawn_disp_y = 2, .pos = { 0.f, sy }, .speed = { 0, 5 } };
  *e++ = { .spawn_disp_y = 2.5f, .pos = { 0.f, sy }, .speed = { 0, 5 } };
  *e++ = { .spawn_disp_y = 3, .pos = { 0.f, sy }, .speed = { 0, 5 } };
  *e++ = { .spawn_disp_y = 3.5f, .pos = { 0.f, sy }, .speed = { 0, 5 } };

  *e++ = { .spawn_disp_y = 5.0f, .pos = { -5.f, sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };
  *e++ = { .spawn_disp_y = 5.5f, .pos = { -5.f, sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };
  *e++ = { .spawn_disp_y = 6.0f, .pos = { -5.f, sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };
  *e++ = { .spawn_disp_y = 6.5f, .pos = { -5.f, sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };

  *e++ = { .spawn_disp_y = 8.0f, .pos = { 5.f, sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
  *e++ = { .spawn_disp_y = 8.5f, .pos = { 5.f, sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
  *e++ = { .spawn_disp_y = 9.0f, .pos = { 5.f, sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
  *e++ = { .spawn_disp_y = 9.5f, .pos = { 5.f, sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
}

struct init {
  init() {
    init_ground_plane();
    init_sky_plane();
    init_enemies();

    input::setup_defaults();
    collision::setup();

    using namespace quack::yakki;
    on_start = [](resources * r) {
      atlas::setup(r);

      ships::on_update = tick;
      ships::setup(r, max_bullets + max_enemies + 1);
    };
    on_frame = [](renderer * r) {
      atlas::run(r);
      ships::run(r);
    };
    start();
  }
} i;
