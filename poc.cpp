#pragma leco app

import casein;
import quack;

static void update_data() {
  quack::donald::data([](auto *& i) {
    *i++ = {
      .position = { 0, 0 },
      .size = { 1, 1 },
      .colour = { 1, 0, 0, 1 },
    };
  });
}

struct init {
  init() {
    using namespace casein;
    using namespace quack::donald;

    app_name("arkas");
    max_quads(1024);

    clear_colour({ 0, 0, 0, 1 });
    // atlas("atlas.png");

    push_constants({
        .grid_pos = { 2, 2 },
        .grid_size = { 4, 4 },
    });

    update_data();
  }
} i;
