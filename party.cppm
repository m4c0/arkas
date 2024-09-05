#pragma leco add_resource "party.png"
export module party;
import dotz;
import hai;
import quack;
import rng;
import sitime;

static constexpr const quack::upc game_area { {}, { 16 } };
static constexpr const auto max_particles = 10240;
static constexpr const auto pi = 3.14159265358979323f;
static constexpr const auto two_pi = 2.f * pi;

static quack::yakki::buffer * g_buffer {};
static quack::yakki::image * g_image {};

struct particle {
  dotz::ivec2 sprite {};
  dotz::vec2 pos {};
  dotz::vec2 speed {};
  dotz::vec2 size {};
  float rot {};
  float rot_speed {};
  float alpha_mult {};
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
      .size = p.size,
      .uv0 = p.sprite / 16.f,
      .uv1 = (p.sprite + 1) / 16.f,
      .multiplier = { 1.f, 1.f, 1.f, p.alpha_mult * p.life },
      .rotation = { p.rot, p.size.x / 2.f, p.size.y / 2.f },
    };
    p.pos = p.pos + p.speed * dt;
    p.size = p.size * (1.0f - dt);
    p.rot += dt * p.rot_speed;
    p.life -= dt;
    if (p.life < 0) p = {};
  }
}

static dotz::vec2 random_circle(float max_r) {
  auto r = rng::randf() * max_r;
  auto th = rng::randf() * two_pi;

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

export namespace party {
  void setup(quack::yakki::resources * r) {
    g_buffer = r->buffer(max_particles, fill_buffer);
    g_buffer->pc() = game_area;
    g_buffer->scissor() = { { -8 }, { 16 }, &game_area };
    g_buffer->start();

    g_image = r->image("party.png");
  }
  void run(quack::yakki::renderer * r) {
    r->run(g_buffer, g_image);
  }
}
export namespace party::fx {
  void fire(dotz::vec2 center, dotz::vec2 speed) {
    emit(100, [=] {
      return particle {
        .sprite = { 0, 1 },
        .pos = random_circle(0.2f) + center,
        .speed = random_circle(3.3f) + speed,
        .size = { 0.1f + rng::randf() * 0.05f },
        .rot = rng::randf() * 360.f,
        .rot_speed = rng::randf() * 2000.f - 1000.f,
        .alpha_mult = 1.0f,
        .life = 0.6f + rng::randf() * 0.4f,
      };
    });
    emit(100, [=] {
      return particle {
        .sprite = { 1, 1 },
        .pos = random_circle(0.5f) + center,
        .speed = random_circle(1.3f) + speed,
        .size = { 0.3f + rng::randf() * 0.05f },
        .rot = rng::randf() * 360.f,
        .rot_speed = rng::randf() * 2000.f - 1000.f,
        .alpha_mult = 1.0f,
        .life = 0.6f + rng::randf() * 0.4f,
      };
    });
  }
  void flash(dotz::vec2 center) {
    emit(1, [=] {
      return particle {
        .sprite = { 2, 1 },
        .pos = center - 0.4f,
        .size = 0.8f,
        .rot = rng::randf() * 360.0f,
        .rot_speed = rng::randf() * 200.0f - 100.0f,
        .alpha_mult = 10.0f,
        .life = 0.1f,
      };
    });
    emit(3, [=] {
      return particle {
        .sprite = { 2, 1 },
        .pos = center - 0.2f,
        .size = 0.4f,
        .rot = rng::rand(8) * 360.f / 8.0f,
        .rot_speed = rng::randf() * 200.0f - 100.0f,
        .alpha_mult = 10.0f,
        .life = 0.1f,
      };
    });
  }
}
