#pragma leco app
#pragma leco add_resource "atlas.png"

import casein;
import quack;

static void update_data(quack::instance *& i) {
  *i++ = {
    .position = { 0, 0 },
    .size = { 1, 1 },
    .colour = { 1, 0, 0, 1 },
  };
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::donald;

    clear_colour({ 0.0f, 0.0f, 0.0f, 1.f });
    push_constants({
        .grid_pos = { 0, 0 },
        .grid_size = { 16, 16 },
    });
    max_quads(1024);

    atlas("atlas.png");
    data(::update_data);
  }
} i;
