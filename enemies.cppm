export module enemies;
import dotz;
import hai;
import ships;

namespace enemies {
  export struct t {
    float spawn_time {};
    dotz::vec2 s0 {};
    dotz::vec2 v0 {};
    dotz::vec2 a {};

    dotz::vec2 pos {};
    dotz::vec2 speed {};
    int life {};
  };
  export constexpr const auto max_enemies = 1024;
  extern hai::array<t> g_enemies;

  export void blit() {
    for (auto & e : g_enemies) {
      if (e.life <= 0) continue;

      ships::blit(e.pos, { 0, 2 });
    }
  }

  export void move_enemies(float t1) {
    for (auto & e : g_enemies) {
      if (e.spawn_time == 0) continue;

      if (e.life > 0) {
        auto t = t1 - e.spawn_time;
        e.pos = e.s0 + e.v0 * t + e.a * t * t * 2.0f;
        e.speed = e.v0 + e.a * t;
      } else if (t1 > e.spawn_time) {
        e.life = 1;
        e.pos = e.s0;
      }
    }
  }

  export void iterate_alive(auto && fn) {
    for (auto & e : g_enemies) {
      if (e.life <= 0) continue;

      if (!fn(e)) break;
    }
  }

  export void reset(auto && fn) {
    g_enemies = hai::array<t> { max_enemies };
    fn(g_enemies.begin());
  }

} // namespace enemies

module :private;
hai::array<enemies::t> enemies::g_enemies { max_enemies };
