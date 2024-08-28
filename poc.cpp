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
  float spawn_time {};
  dotz::vec2 pos {};
  dotz::vec2 speed {};
  dotz::vec2 accel {};
  int life {};
};
static constexpr const auto max_enemies = 1024;
static hai::array<enemy> g_enemies { max_enemies };

static dotz::vec2 player_pos { -0.5f, 3.5f };
static sitime::stopwatch timer {};

static constexpr const float initial_displ_y = atlas::initial_displ_y;
static sitime::stopwatch g_lvl_timer {};

static void update_data() {
  for (auto & e : g_enemies) {
    if (e.life <= 0) continue;

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
  auto displ_y = initial_displ_y - g_lvl_timer.millis() / 1000.0f;
  atlas::parallax(displ_y, player_pos);
}

static void move_enemies(float dt) {
  auto t = g_lvl_timer.millis() / 1000.0f;
  for (auto & e : g_enemies) {
    if (e.spawn_time == 0) continue;

    if (e.life > 0) {
      e.pos = e.pos + e.speed * dt;
      e.speed = e.speed + e.accel * dt;
    } else if (t > e.spawn_time) {
      e.life = 1;
    }
  }
}

static void check_bullet_enemy_collisions() {
  for (auto & b : g_bullets) {
    if (!b.active) continue;

    for (auto & e : g_enemies) {
      if (e.life <= 0) continue;

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

  *e++ = { .spawn_time = 2, .pos = { 0.f, sy }, .speed = { 0, 5 } };
  *e++ = { .spawn_time = 2.5f, .pos = { 0.f, sy }, .speed = { 0, 5 } };
  *e++ = { .spawn_time = 3, .pos = { 0.f, sy }, .speed = { 0, 5 } };
  *e++ = { .spawn_time = 3.5f, .pos = { 0.f, sy }, .speed = { 0, 5 } };

  *e++ = { .spawn_time = 5.0f, .pos = { -5.f, sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };
  *e++ = { .spawn_time = 5.5f, .pos = { -5.f, sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };
  *e++ = { .spawn_time = 6.0f, .pos = { -5.f, sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };
  *e++ = { .spawn_time = 6.5f, .pos = { -5.f, sy }, .speed = { 0, 5 }, .accel = { 1, 0 } };

  *e++ = { .spawn_time = 8.0f, .pos = { 5.f, sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
  *e++ = { .spawn_time = 8.5f, .pos = { 5.f, sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
  *e++ = { .spawn_time = 9.0f, .pos = { 5.f, sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
  *e++ = { .spawn_time = 9.5f, .pos = { 5.f, sy }, .speed = { 0, 5 }, .accel = { -1, 0 } };
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
