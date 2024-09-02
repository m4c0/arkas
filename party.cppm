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

struct emitter {
  dotz::vec2 center {};
  float radius {};
  float life_min {};
  float life_max {};
  unsigned count {};
};
static void emit(const emitter & e) {
  auto qty = e.count;
  for (auto i = 0; i < max_particles && qty > 0; i++) {
    auto n = (i + g_last_emitted) % max_particles;
    if (g_parts[n].life > 0) continue;

    auto p = random_circle(e.radius);
    auto life = e.life_min + rng::randf() * (e.life_max - e.life_min);

    g_parts[n] = particle {
      .pos = e.center + p,
      .life = life,
    };
    g_last_emitted = n;
    qty--;
  }
}

static void on_timer() {
  emit({
    .center = { rng::randf() * 2.5f, rng::randf() * 2.5f },
    .life_min = 0.5f,
    .life_max = 0.5f,
    .count = 1,
  });
  emit({
    .center = { -3 + rng::randf() * 1.5f, rng::randf() * 1.5f },
    .radius = 0.3f,
    .life_min = 0.3f,
    .life_max = 0.9f,
    .count = 10,
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

    casein::handle(casein::TIMER, on_timer);
  }
} i;
