#pragma leco app
#pragma leco add_resource "party.png"
export module party;
import casein;
import dotz;
import hai;
import quack;
import rng;
import sitime;

static constexpr const auto max_particles = 1024;

static quack::yakki::buffer * g_buffer {};
static quack::yakki::image * g_image {};

struct particle {
  dotz::vec2 pos {};
  float life {};
};
static hai::array<particle> g_parts { max_particles };

static sitime::stopwatch g_time_since_last_frame {};
static int g_last_emitted {};

static void fill_buffer(quack::instance *& i) {
  float dt = g_time_since_last_frame.millis() / 1000.f;
  g_time_since_last_frame = {};

  for (auto & p : g_parts) {
    *i++ = {
      .position = p.pos,
      .size = { 0.1f },
      .colour = { 1.f * p.life },
    };
    p.life -= dt;
    if (p.life < 0) p.life = 0;
  }
}

static void on_timer() {
  for (auto i = 0; i < max_particles; i++) {
    auto n = (i + g_last_emitted) % max_particles;
    if (g_parts[n].life == 0) {
      g_parts[n] = particle {
        .pos = { rng::randf() * 2.5f, rng::randf() * 2.5f },
        .life = 0.5,
      };
      g_last_emitted = n;
      break;
    }
  }
}

struct init {
  init() {
    using namespace quack::yakki;

    on_start = [](auto * r) {
      g_buffer = r->buffer(max_particles, fill_buffer);
      g_buffer->pc() = { { 0 }, { 16 } };
      g_buffer->start();

      g_image = r->image("party.png");
    };
    on_frame = [](auto * r) {
      r->run(g_buffer, g_image);
    };
    start();

    casein::handle(casein::TIMER, on_timer);
  }
} i;
