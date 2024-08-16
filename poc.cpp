#pragma leco app
#pragma leco add_resource "atlas.png"

import casein;
import dotz;
import input;
import quack;
import sitime;
import sith;
import voo;

static dotz::vec2 player_pos {};
static sitime::stopwatch timer {};

static void update_data(quack::instance * i) {
  *i++ = {
    .position = player_pos,
    .size = { 1, 1 },
    .colour = { 1, 0, 1, 1 },
  };
}

static void move_player(float dt) {
  auto d = input::left_stick();
  if (dotz::length(d) < 0.001) return;

  player_pos = player_pos + d * dt * 10.0;
}

static void repaint(quack::instance * i) {
  float dt = timer.millis() / 1000.f;
  timer = {};

  move_player(dt);
  update_data(i);
}

class init : public voo::casein_thread {
public:
  void run() override {
    input::setup_defaults();

    voo::device_and_queue dq { "arkas" };

    quack::pipeline_stuff ps { dq, 100 };
    quack::buffer_updater u { &dq, 1024, &repaint };
    u.run_once();

    sith::run_guard rg { &u }; // For animation

    quack::image_updater a { &dq, &ps, [](auto pd) { return voo::load_sires_image("atlas.png", pd); } };
    a.run_once();

    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq };

      quack::upc rpc {
        .grid_pos = { 0, -6 },
        .grid_size = { 16, 16 },
      };

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass({
              .command_buffer = *pcb,
              .clear_color = { { 0, 0, 0, 1 } },
          });
          ps.run({
              .sw = &sw,
              .scb = *scb,
              .pc = &rpc,
              .inst_buffer = u.data().local_buffer(),
              .atlas_dset = a.dset(),
              .count = 1,
          });
        });
      });
    }
  }
} i;
