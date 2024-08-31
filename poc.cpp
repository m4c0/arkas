#pragma leco app

import atlas;
import collision;
import dotz;
import enemies;
import hai;
import input;
import plane;
import quack;
import rng;
import ships;
import sitime;
import voo;

static constexpr const char ground_data[plane::t::h][plane::t::w + 1] {
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000111111111111",
  "0011111100011111",
  "0111100000111100",
  "0111100000111000",
};
static constexpr const plane::t ground_plane = [] {
  plane::t res {};
  for (auto y = 0; y < plane::t::h; y++) {
    for (auto x = 0; x < plane::t::w; x++) {
      auto & p = res.at({ x, y });
      switch (ground_data[y][x]) {
        case '0': p = plane::at_water; break;
        case '1': p = plane::at_grass; break;
        default: throw 0;
      }
    }
  }
  return res;
}();

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

static void init_ground_plane() {
  for (auto y = 0; y < plane::t::h; y++) {
    for (auto x = 0; x < plane::t::w; x++) {
      atlas::ground({ x, y }) = ground_plane.at({ x, y });
    }
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
  enemies::reset([](auto * e) {
    constexpr const auto sy = atlas::min_area_y - 1;

    *e++ = { .spawn_time = 2, .s0 = { 0.f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 2.5f, .s0 = { 0.f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 3, .s0 = { 0.f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 3.5f, .s0 = { 0.f, sy }, .v0 = { 0, 5 } };

    *e++ = { .spawn_time = 5.0f, .s0 = { -5.f, sy }, .v0 = { 0, 5 }, .a = { 1, 0 } };
    *e++ = { .spawn_time = 5.5f, .s0 = { -5.f, sy }, .v0 = { 0, 5 }, .a = { 1, 0 } };
    *e++ = { .spawn_time = 6.0f, .s0 = { -5.f, sy }, .v0 = { 0, 5 }, .a = { 1, 0 } };
    *e++ = { .spawn_time = 6.5f, .s0 = { -5.f, sy }, .v0 = { 0, 5 }, .a = { 1, 0 } };

    *e++ = { .spawn_time = 8.0f, .s0 = { 5.f, sy }, .v0 = { 0, 5 }, .a = { -1, 0 } };
    *e++ = { .spawn_time = 8.5f, .s0 = { 5.f, sy }, .v0 = { 0, 5 }, .a = { -1, 0 } };
    *e++ = { .spawn_time = 9.0f, .s0 = { 5.f, sy }, .v0 = { 0, 5 }, .a = { -1, 0 } };
    *e++ = { .spawn_time = 9.5f, .s0 = { 5.f, sy }, .v0 = { 0, 5 }, .a = { -1, 0 } };
  });
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
      ships::setup(r, max_bullets + enemies::max_enemies + 1);
    };
    on_frame = [](renderer * r) {
      atlas::run(r);
      ships::run(r);
    };
    start();
  }
} i;
