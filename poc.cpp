#pragma leco app

import atlas;
import collision;
import dotz;
import enemies;
import hai;
import input;
import level;
import party;
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

static dotz::vec2 player_pos { -0.5f, 3.5f };
static sitime::stopwatch timer {};

static constexpr const float initial_displ_y = atlas::initial_displ_y;
static sitime::stopwatch g_lvl_timer {};

static void update_data() {
  enemies::blit();

  for (auto & b : g_bullets) {
    if (!b.active) continue;

    ships::blit(b.pos, { 1, 0 });
  }

  ships::blit(player_pos, { 0, 1 });
}

static void move_player(float dt) {
  static constexpr const auto speed = 10.0;

  auto d = input::left_stick();
  if (dotz::length(d) < 0.001) return;

  player_pos = dotz::clamp(player_pos + d * dt * speed, { -8 }, { 7 });
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

static void parallax(float t1) {
  // TODO: fix upper bound
  // TODO: trigger "end level" when upper bound is reached
  auto displ_y = initial_displ_y - t1;
  atlas::parallax(displ_y, player_pos);
}

static void check_bullet_enemy_collisions() {
  for (auto & b : g_bullets) {
    if (!b.active) continue;

    enemies::iterate_alive([&](auto & e) {
      if (!collision::between(e.pos, { 0, 2 }, b.pos, { 1, 0 })) return true;

      party::fx::fire(e.pos + 0.5, e.speed);

      e = {};
      b = {};
      return false;
    });
  }
}

static void tick() {
  float dt = timer.millis() / 1000.f;
  timer = {};

  auto t1 = g_lvl_timer.millis() / 1000.0f;

  check_bullet_enemy_collisions();
  move_player(dt);
  shoot(dt);
  move_bullets(dt);

  enemies::move_enemies(t1);
  parallax(t1);
  update_data();
}

struct init {
  init() {
    level::load();

    input::setup_defaults();
    collision::setup();

    using namespace quack::yakki;
    on_start = [](resources * r) {
      atlas::setup(r);
      party::setup(r);

      ships::on_update = tick;
      ships::setup(r, max_bullets + enemies::max_enemies + 1);
    };
    on_frame = [](renderer * r) {
      atlas::run(r);
      ships::run(r);
      party::run(r);
    };
    start();
  }
} i;
