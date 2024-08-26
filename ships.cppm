#pragma leco add_impl ships_impl
#pragma leco add_resource "ships.png"
export module ships;
import dotz;
import hai;
import quack;

export namespace ships {
  void blit(dotz::vec2 pos, dotz::ivec2 uv);
  void setup(quack::yakki::resources * r, unsigned quad_count);
  void run(quack::yakki::renderer * r);

  extern hai::fn<void> on_update;
} // namespace ships
