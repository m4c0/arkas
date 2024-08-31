#pragma leco add_resource "atlas.png"
export module atlas;

import dotz;
import plane;
import quack;

static constexpr const auto hpw = plane::t::draw_w / 2.f;
static constexpr const quack::upc game_area { {}, { 16 } };

static quack::yakki::buffer * g_gnd_plane_buffer;
static quack::yakki::buffer * g_sky_plane_buffer;
static quack::yakki::image * g_plane_image;

static plane::t g_gnd_plane {};
static plane::t g_sky_plane {};

export namespace atlas {
  constexpr const float initial_displ_y = plane::t::draw_h - 2;
  constexpr const float max_area_y = game_area.grid_size.y / 2;
  constexpr const float min_area_y = -max_area_y;

  void parallax(float displ_y, dotz::vec2 player_pos) {
    auto plane_dx = 2.0f * (player_pos.x + 0.5f) / 8.0f;
    auto plane_dy = 2.0f * (player_pos.y + 0.5f) / 8.0f;

    g_gnd_plane_buffer->pc().grid_pos = {
      plane_dx + plane::t::draw_w / 2.0f,
      plane_dy + displ_y - hpw + 2.f,
    };
    g_gnd_plane_buffer->pc().grid_size = { plane::t::draw_w - 4 };

    plane_dx = 8.0f * (player_pos.x + 0.5f) / 8.0f;
    g_sky_plane_buffer->pc().grid_pos = {
      plane_dx + plane::t::draw_w / 2.0f,
      plane_dy + displ_y - hpw + 8.f,
    };
    g_sky_plane_buffer->pc().grid_size = { plane::t::draw_w - 16 };
  }

  void setup(quack::yakki::resources * r) {
    quack::scissor s {
      .offset = -game_area.grid_size / 2,
      .extent = game_area.grid_size,
      .ref = &game_area,
    };

    g_gnd_plane_buffer = r->buffer(plane::t::tiles, [](auto *& i) { plane::render(&g_gnd_plane, i); });
    g_gnd_plane_buffer->pc() = {
      .grid_pos = { hpw, initial_displ_y - hpw + 2 },
      .grid_size = { plane::t::draw_w - 4 },
    };
    g_gnd_plane_buffer->scissor() = s;

    g_sky_plane_buffer = r->buffer(plane::t::tiles, [](auto *& i) { plane::render(&g_sky_plane, i); });
    g_sky_plane_buffer->pc() = {
      .grid_pos = { hpw, initial_displ_y - hpw + 8 },
      .grid_size = { plane::t::draw_w - 16 },
    };
    g_sky_plane_buffer->scissor() = s;

    g_plane_image = r->image("atlas.png");
  }

  void run(quack::yakki::renderer * r) {
    r->run(g_gnd_plane_buffer, g_plane_image);
    r->run(g_sky_plane_buffer, g_plane_image);
  }

  namespace buffers {
    [[nodiscard]] auto * ground() { return g_gnd_plane_buffer; }
    [[nodiscard]] auto * sky() { return g_sky_plane_buffer; }
  }
  [[nodiscard]] auto * image() { return g_plane_image; }

  [[nodiscard]] auto & ground(dotz::ivec2 p) { return g_gnd_plane.at(p); }
  [[nodiscard]] auto & sky(dotz::ivec2 p) { return g_sky_plane.at(p); }
} // namespace atlas
