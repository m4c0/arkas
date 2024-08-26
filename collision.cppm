#pragma leco add_impl collision_impl
export module collision;
import dotz;

export namespace collision {
  bool between(dotz::vec2 b_pos, dotz::ivec2 b_uv, dotz::vec2 e_pos, dotz::ivec2 e_uv);

  void setup();
}; // namespace collision
