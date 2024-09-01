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
  "0011111111111111",
  "0011122222221111",
  "0011112222221111",
  "0001122222211000",
  "0000122222210000",
  "0000122222110000",
  "0001111111100000",
  "0000111111110000",
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
        case '2': p = plane::at_dirt; break;
        default: throw 0;
      }
    }
  }
  return res;
}();

static constexpr const char sky_data[plane::t::h][plane::t::w + 1] {
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0001000000000000",
  "0000010000000000",
  "0001000000000000",
  "0000010000000000",
  "0001000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000111000000000",
  "0000000000000000",
  "0000000000011100",
  "0000000000000000",
  "0000000000000000",
  "0000000001111100",
  "0000000011111000",
  "0000000000011111",
  "0000000000000000",
  "0111000000000000",
  "0011100000000000",
  "0001111100000000",
  "0111111000000000",
  "0000000000000000",
  "0000000000000000",
};
static constexpr const plane::t sky_plane = [] {
  plane::t res {};
  for (auto y = 0; y < plane::t::h; y++) {
    for (auto x = 0; x < plane::t::w; x++) {
      auto & p = res.at({ x, y });
      switch (sky_data[y][x]) {
        case '0': p = plane::at_void; break;
        case '1': p = plane::at_cloud; break;
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

static void init_enemies() {
  enemies::reset([](auto * e) {
    constexpr const auto sy = atlas::min_area_y - 1;

    *e++ = { .spawn_time = 2, .s0 = { -0.5f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 2.5f, .s0 = { -0.5f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 3, .s0 = { -0.5f, sy }, .v0 = { 0, 5 } };
    *e++ = { .spawn_time = 3.5f, .s0 = { -0.5f, sy }, .v0 = { 0, 5 } };

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
    atlas::planes::ground() = ground_plane;
    atlas::planes::sky() = sky_plane;
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
