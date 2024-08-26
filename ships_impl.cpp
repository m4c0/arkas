module ships;

static constexpr const quack::upc game_area { {}, { 16 } };

hai::fn<void> ships::on_update {};

static quack::yakki::buffer * g_buffer;
static quack::yakki::image * g_image;

static quack::instance * g_i;

static void setup_buffer(quack::instance *& i) {
  g_i = i;
  ships::on_update();
  i = g_i;
}

void ships::setup(quack::yakki::resources * r, unsigned quad_count) {
  g_buffer = r->buffer(quad_count, setup_buffer);
  g_buffer->pc() = game_area;
  g_buffer->start();

  g_image = r->image("ships.png");
}

void ships::run(quack::yakki::renderer * r) { r->run(g_buffer, g_image); }

void ships::blit(dotz::vec2 pos, dotz::ivec2 uv) {
  *g_i++ = {
    .position = pos,
    .size = { 1, 1 },
    .uv0 = uv / 16.f,
    .uv1 = (uv + 1.f) / 16.f,
    .multiplier = { 1 },
  };
}

dotz::vec2 ships::mouse_pos() { return g_buffer->mouse_pos(); }
