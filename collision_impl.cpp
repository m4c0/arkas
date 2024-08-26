module collision;
import pixed;
import traits;

using namespace traits::ints;

namespace {
  struct mask {
    unsigned rows[16] {};
  };
  static mask g_masks[16][16] {};
} // namespace

bool collision::between(dotz::vec2 b_pos, dotz::ivec2 b_uv, dotz::vec2 e_pos, dotz::ivec2 e_uv) {
  auto bs = b_pos;
  auto be = bs + 1;

  auto es = e_pos;
  auto ee = es + 1;

  if (ee.x < bs.x || es.x > be.x) return false;
  if (ee.y < bs.y || es.y > be.y) return false;

  const auto & e_msk = g_masks[e_uv.y][e_uv.x];
  const auto & b_msk = g_masks[b_uv.y][b_uv.x];

  int dx = (es.x - bs.x) * 16;
  // Should not happen, but FP precision am I right?
  if (dx <= -16 || dx >= 16) return false;

  for (int y = 0; y < 16; y++) {
    int dy = es.y * 16 + y - bs.y * 16;
    if (dy < 0 || dy >= 16) continue;

    uint64_t em = e_msk.rows[y] << 16;
    uint64_t bm = b_msk.rows[dy] << (16 + dx);
    if ((em & bm) != 0) return true;
  }

  return false;
}

static void create_bitmaps(const pixed::context & ctx) {
  for (auto y = 0, p = 0; y < ctx.h; y++) {
    for (auto x = 0; x < ctx.w; x++, p++) {
      auto sx = x / 16;
      auto sy = y / 16;
      auto px = x % 16;
      auto py = y % 16;

      if (ctx.image[p].a < 128) continue;

      auto & row = g_masks[sy][sx].rows[py];
      row |= 1 << (15 - px);
    }
  }
}

void collision::setup() {
  pixed::read("ships.png").map(create_bitmaps).log_error([] { throw 1; });
}
