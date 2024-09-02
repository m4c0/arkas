#pragma leco app
#pragma leco add_resource "party.png"
export module party;
import casein;
import dotz;
import hai;
import quack;
import rng;
import sitime;

static constexpr const auto max_particles = 10240;

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

static dotz::vec2 random_circle(float max_r) {
  auto r = rng::randf() * max_r;
  auto th = rng::randf() * 3.14159265358979323f * 2.0f;

  float x = r * dotz::sin(th);
  float y = r * dotz::cos(th);
  return { x, y };
}

static void emit(unsigned qty, auto && fn) {
  for (auto i = 0; i < max_particles && qty > 0; i++) {
    auto n = (i + g_last_emitted) % max_particles;
    if (g_parts[n].life > 0) continue;

    g_parts[n] = fn();
    g_last_emitted = n;
    qty--;
  }
}

static void fire() {
  emit(100, [] {
    return particle {
      .pos = random_circle(0.3f),
      .life = 0.6f + rng::randf() * 0.3f,
    };
  });
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

    casein::handle(casein::KEY_DOWN, casein::K_SPACE, fire);
  }
} i;
