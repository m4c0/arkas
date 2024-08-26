#pragma leco app
#pragma leco add_resource "ships.png"

import dotz;
import quack;

static constexpr const dotz::vec2 enemy_pos { -2, -4 };
static dotz::vec2 g_player_pos { 2, 4 };

static quack::yakki::buffer * g_buffer;
static quack::yakki::image * g_image;

static constexpr auto uv(int x, int y) { return dotz::vec2 { x, y } / 16.f; }

static void setup_buffer(quack::instance *& i) {
  *i++ = {
    .position = enemy_pos,
    .size = { 1 },
    .uv0 = uv(0, 2),
    .uv1 = uv(0, 2) + 1.f / 16.f,
    .multiplier = { 1 },
  };

  *i++ = {
    .position = g_player_pos,
    .size = { 1, 1 },
    .uv0 = uv(0, 1),
    .uv1 = uv(0, 1) + 1.f / 16.f,
    .multiplier = { 1 },
  };
}

struct init {
  init() {
    using namespace quack::yakki;
    on_start = [](auto * r) {
      g_buffer = r->buffer(100, setup_buffer);
      g_buffer->pc() = { .grid_size = { 16 } };
      g_buffer->start();

      g_image = r->image("ships.png");
    };
    on_frame = [](auto * r) { r->run(g_buffer, g_image); };
    start();
  }
} i;
