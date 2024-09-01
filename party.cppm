#pragma leco app
#pragma leco add_resource "party.png"
export module party;
import casein;
import quack;

static quack::yakki::buffer * g_buffer {};
static quack::yakki::image * g_image {};

static void fill_buffer(quack::instance *& i) {
}

struct init {
  init() {
    using namespace quack::yakki;

    on_start = [](auto * r) {
      g_buffer = r->buffer(1024, fill_buffer);
      g_image = r->image("party.png");
    };
    on_frame = [](auto * r) {
      r->run(g_buffer, g_image);
    };
    start();
  }
} i;
