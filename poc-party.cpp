#pragma leco app

import casein;
import party;
import quack;

struct init {
  init() {
    using namespace quack::yakki;

    on_start = party::setup;
    on_frame = party::run;
    start();

    casein::window_title = "Particle FX testbench";
    casein::handle(casein::KEY_DOWN, casein::K_SPACE, [] {
      party::fx::fire({}, { 1, 1 });
    });
  }
} i;

