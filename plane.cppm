export module plane;
import dotz;
import quack;

namespace plane {
  export enum area_type { at_void = 0, at_water, at_grass, at_dirt, at_cloud };

  static constexpr dotz::ivec2 uv0(area_type a, area_type b) {
    switch (a) {
      case at_void:
        if (b == at_void) return { 1, 5 };
        if (b == at_cloud) return { 1, 5 };
        break;
      case at_water:
        if (b == at_water) return { 1, 2 };
        if (b == at_grass) return { 1, 2 };
        break;
      case at_grass:
        if (b == at_grass) return { 4, 2 };
        if (b == at_water) return { 4, 2 };
        if (b == at_dirt) return { 7, 2 };
        break;
      case at_dirt:
        if (b == at_dirt) return { 10, 2 };
        if (b == at_grass) return { 10, 2 };
        break;
      case at_cloud:
        if (b == at_cloud) return { 4, 5 };
        if (b == at_void) return { 4, 5 };
        break;
      default: break;
    }
    return {};
  }
  static constexpr dotz::ivec2 uv0(area_type a, area_type b, dotz::ivec2 d) {
    auto p = uv0(a, b);
    return p == 0 ? 0 : p + d;
  }
  export inline constexpr dotz::ivec2 uv0(area_type a) { return uv0(a, a); }

  export inline void blit(quack::instance *& i, dotz::vec2 p, dotz::ivec2 uv0) {
    if (uv0 == 0) return;

    *i++ = {
      .position = p,
      .size = { 1 },
      .uv0 = uv0 / 16.f,
      .uv1 = (uv0 + 1) / 16.f,
      .multiplier = { 1 },
    };
  }

  export class t {
  public:
    static constexpr const auto w = 16;
    static constexpr const auto h = 32;

    static constexpr const auto draw_w = w * 2 - 1;
    static constexpr const auto draw_h = h * 2 - 1;

    static constexpr const auto tiles = draw_w * draw_h;

  private:
    area_type m_seeds[h][w] {};

  public:
    constexpr auto & at(dotz::ivec2 p) { return m_seeds[p.y][p.x]; }
  };

  export void render(plane::t * pl, quack::instance *& i) {
    for (dotz::ivec2 p {}; p.y < t::h; p.y++) {
      for (p.x = 0; p.x < t::w; p.x++) {
        const auto blt = [&](dotz::ivec2 d, dotz::ivec2 uv0) { blit(i, p * 2 + d, uv0); };

        blt(0, uv0(pl->at(p)));

        constexpr const dotz::ivec2 dr { 1, 0 };
        constexpr const dotz::ivec2 db { 0, 1 };
        if (p.x < t::w - 1) {
          auto l = pl->at(p);
          auto r = pl->at(p + dr);
          if (l == r) blt(dr, uv0(l, r));
          else blt(dr, uv0(l, r, dr));
        }
        if (p.y < t::h - 1) {
          auto t = pl->at(p);
          auto b = pl->at(p + db);
          if (t == b) blt(db, uv0(t, b));
          else blt(db, uv0(t, b, db));
        }
        if (p.x < t::w - 1 && p.y < t::h - 1) {
          auto tl = pl->at(p);
          auto tr = pl->at(p + dr);
          auto bl = pl->at(p + db);
          auto br = pl->at(p + 1);
          if (tl == tr && tl == bl && tl == br) blt(1, uv0(tl));
          else if (tl == tr && bl == br) blt(1, uv0(tl, bl, db));
          else if (tl == bl && tr == br) blt(1, uv0(tl, tr, dr));
          else if (tl == br && tr == bl) continue;
          else if (tl == tr && tl == bl) blt(1, uv0(br, tl, -dr - db));
          else if (tl == tr && tl == br) blt(1, uv0(bl, tl, dr - db));
          else if (tl == bl && tl == br) blt(1, uv0(tr, tl, -dr + db));
          else if (br == bl && br == tr) blt(1, uv0(tl, br, dr + db));
        }
      }
    }
  }
} // namespace plane
