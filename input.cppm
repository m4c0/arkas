export module input;
import casein;
import dotz;

namespace input {
  export enum buttons { B_UP = 0, B_DOWN, B_LEFT, B_RIGHT, B_COUNT };
  static bool button_state[B_COUNT] {};

  static float axis(buttons n, buttons p) {
    auto ns = button_state[n];
    auto ps = button_state[p];

    if (ns && !ps) return -1;
    if (ps && !ns) return +1;
    return 0;
  }

  export void handle(casein::keys k, buttons b) {
    casein::handle(casein::KEY_DOWN, k, [=] { button_state[b] = true; });
    casein::handle(casein::KEY_UP, k, [=] { button_state[b] = false; });
  }

  export dotz::vec2 left_stick() { return dotz::vec2 { axis(B_LEFT, B_RIGHT), axis(B_UP, B_DOWN) }; }

  export void setup_defaults() {
    handle(casein::K_W, B_UP);
    handle(casein::K_S, B_DOWN);
    handle(casein::K_A, B_LEFT);
    handle(casein::K_D, B_RIGHT);
  }
} // namespace input
