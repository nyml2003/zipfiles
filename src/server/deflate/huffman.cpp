#include "server/deflate/huffman.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

namespace zipfiles::server::huffman {

// generate std huffman code
template <size_t size>
constexpr void gen_huffman_code(
  std::array<uint16_t, size>& huffman_code,
  const std::array<uint8_t, size>& bit_len,
  const std::array<int, MAX_BIT_LEN + 1>& bit_len_count
) {
  int char_count = size;
  std::array<int, size> next_code{};  // next code for each bit len

  // gen next code
  for (int i = 1, code = 0; i <= MAX_BIT_LEN + 1; ++i) {
    next_code[i] = code = (code + bit_len_count[i - 1]) << 1;
  }

  // gen huffman code
  for (int i = 0, len = 0; i < char_count; ++i) {
    len = bit_len[i];
    if (!len) {
      continue;
    }
    huffman_code[i] = next_code[len]++;
  }
}

namespace init {
// bit length of extra bits of length code
constexpr std::array<int, LEN_CODE_COUNT> LEN_EXTRA_BITS = {
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2,
  2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
};
constexpr std::pair<std::array<int, LEN_CODE_COUNT>, std::array<int, MAX_LEN>>
GEN_LENGTH_CODE() {
  std::array<int, LEN_CODE_COUNT> base_length{};
  std::array<int, MAX_LEN> length_code{};
  for (int code = 0, length = 0; code < LEN_CODE_COUNT - 1; code++) {
    base_length[code] = length;
    for (int i = 0; i < (1 << LEN_EXTRA_BITS[code]); i++) {
      length_code[length++] = code;
    }
  }
  base_length.back() = 255;
  length_code.back() = LEN_CODE_COUNT - 1;
  return {base_length, length_code};
}

// table of length code to length base
constexpr std::array<int, LEN_CODE_COUNT> LEN_BASE_TABLE =
  GEN_LENGTH_CODE().first;

// table of length to length code
constexpr std::array<int, MAX_LEN> LEN_CODE_TABLE = GEN_LENGTH_CODE().second;

// bit length of extra bits of distance code
constexpr std::array<int, DIST_CODE_COUNT> DIST_EXTRA_BITS = {
  0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6,
  6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
};
constexpr std::
  pair<std::array<int, DIST_CODE_COUNT>, std::array<int, DIST_CODE_TABLE_SIZE>>
  GEN_DISTANCE_CODE() {
  std::array<int, DIST_CODE_COUNT> base_distance{};
  std::array<int, DIST_CODE_TABLE_SIZE> distance_code{};
  int dist = 0;
  for (int code = 0; code < 16; code++) {
    base_distance[code] = dist;
    for (int i = 0; i < (1 << DIST_EXTRA_BITS[code]); i++) {
      distance_code[dist++] = code;
    }
  }
  dist >>= 7;
  for (int code = 16; code < DIST_CODE_COUNT; code++) {
    base_distance[code] = dist << 7;
    for (int i = 0; i < (1 << (DIST_EXTRA_BITS[code] - 7)); i++) {
      distance_code[256 + dist++] = code;
    }
  }
  return {base_distance, distance_code};
}

// table of distance code to distance base
constexpr std::array<int, DIST_CODE_COUNT> DIST_BASE_TABLE =
  GEN_DISTANCE_CODE().first;

// table of distance to distance code
constexpr std::array<int, DIST_CODE_TABLE_SIZE> DIST_CODE_TABLE =
  GEN_DISTANCE_CODE().second;

constexpr Tree<LC_TREE_SIZE> GEN_STATIC_LITERAL_LENGTH_TREE() {
  Tree<LC_TREE_SIZE> tree;
  std::array<int, MAX_BIT_LEN + 1> bit_len_count{};
  for (auto& i : bit_len_count) {
    i = 0;
  }
  int character = 0;
  while (character <= 143) {
    tree.bit_len[character++] = 8, bit_len_count[8]++;
  }
  while (character <= 255) {
    tree.bit_len[character++] = 9, bit_len_count[9]++;
  }
  while (character <= 279) {
    tree.bit_len[character++] = 7, bit_len_count[7]++;
  }
  while (character < 287) {
    tree.bit_len[character++] = 8, bit_len_count[8]++;
  }
  gen_huffman_code(tree.code, tree.bit_len, bit_len_count);
  return tree;
}

// static length or character huffman tree
constexpr Tree<LC_TREE_SIZE> STATIC_LC_TREE = GEN_STATIC_LITERAL_LENGTH_TREE();

constexpr Tree<DIST_TREE_SIZE> GEN_STATIC_DISTANCE_TREE() {
  Tree<DIST_TREE_SIZE> tree;
  for (int i = 0; i < DIST_CODE_COUNT; i++) {
    tree.bit_len[i] = 5;
    tree.code[i] = i;
  }
  return tree;
}

// static distance huffman tree
constexpr Tree<DIST_TREE_SIZE> STATIC_DIST_TREE = GEN_STATIC_DISTANCE_TREE();
}  // namespace init

using init::DIST_BASE_TABLE;   // table of distance code to distance base
using init::DIST_CODE_TABLE;   // table of distance to distance code
using init::DIST_EXTRA_BITS;   // bit length of extra bits of distance code
using init::LEN_BASE_TABLE;    // table of length code to length base
using init::LEN_CODE_TABLE;    // table of length to length code
using init::LEN_EXTRA_BITS;    // bit length of extra bits of length code
using init::STATIC_DIST_TREE;  // static distance huffman tree
using init::STATIC_LC_TREE;    // static length or character huffman tree

// transfrom distance to dist_code
uint16_t get_dist_code(int dist) {
  --dist;
  assert(dist >= 0);
  return DIST_CODE_TABLE[dist < 256 ? dist : 256 + (dist >> 7)];
}

template <typename HeapType>
inline void clear_heap(HeapType& heap) {
  while (!heap.empty()) {
    heap.pop();
  }
}

template <size_t size, typename HeapType>
void init_heap(std::array<int, size>& frequency, HeapType& heap) {
  int leaves = size / 2;  // [0,leaves) are leaves of the huffman tree
  for (int i = 0; i < leaves; ++i) {
    if (frequency[i]) {
      heap.push(i);
    }
  }
  int nonzero = 0;
  while (heap.size() < 2) {
    while (frequency[nonzero]) {
      ++nonzero;
    }
    frequency[nonzero] = 1;
    heap.push(nonzero++);
  }
}

// build raw huffman tree without limit the height
template <size_t size, typename HeapType>
int build_raw_tree(
  std::array<int, size>& frequency,
  std::array<int, size>& father,
  HeapType& heap
) {
  int top = size / 2;
  int lson = 0;
  int rson = 0;
  while (heap.size() >= 2) {
    lson = heap.top();
    heap.pop();
    rson = heap.top();
    heap.pop();
    frequency[top] = frequency[lson] + frequency[rson];
    father[lson] = father[rson] = top;
    heap.push(top++);
  }
  return top;
}

// generate the bit length (compressed the hight of the tree if needed)
template <size_t size>
void gen_bit_len(
  const std::array<int, size>& father,
  std::array<uint8_t, size>& bit_len,
  std::array<int, MAX_BIT_LEN + 1>& bit_len_count,
  int top
) {
  int overflow = 0;
  int leaves = size / 2;
  while (top--) {
    if (father[top]) {
      bit_len[top] = bit_len[father[top]] + 1;
      if (top < leaves) {
        if (bit_len[top] <= MAX_BIT_LEN) {
          ++bit_len_count[bit_len[top]];
        } else {
          ++bit_len_count[MAX_BIT_LEN];
          ++overflow;
        }
      }
    }
  }
  int max_free_bit_len = 0;
  while (overflow) {
    max_free_bit_len = MAX_BIT_LEN - 1;
    while (!bit_len_count[max_free_bit_len]) {
      --max_free_bit_len;
    }
    --bit_len_count[max_free_bit_len];
    --bit_len_count[MAX_BIT_LEN];
    bit_len_count[max_free_bit_len + 1] += 2;
    --overflow;
  }
}

// rebuild huffman tree after compressed the bit len
template <size_t size, typename HeapType>
void rebuild_tree(
  const std::array<int, MAX_BIT_LEN + 1>& bit_len_count,
  std::array<uint8_t, size>& bit_len,
  HeapType& heap
) {
  int leaves = size / 2;
  for (int i = 0; i < leaves; ++i) {
    if (bit_len[i]) {
      heap.push(i);
    }
  }
  for (int i = MAX_BIT_LEN; i > 0; --i) {
    for (int j = 0; j < bit_len_count[i]; ++j) {
      bit_len[heap.top()] = i;
      heap.pop();
    }
  }
  assert(heap.empty());
}

template <size_t size>
void Tree<size>::build_tree() {
  auto cmp = [this](const int a, const int b) {
    return frequency[a] > frequency[b];
  };
  std::priority_queue<int, std::vector<int>, decltype(cmp)> heap(cmp);

  init_heap(frequency, heap);

  // top := huffman tree's node count
  int top = build_raw_tree(frequency, father, heap);

  std::array<int, MAX_BIT_LEN + 1> bit_len_count{};
  gen_bit_len(father, bit_len, bit_len_count, top);

  clear_heap(heap);

  rebuild_tree(bit_len_count, bit_len, heap);

  gen_huffman_code(code, bit_len, bit_len_count);
}

template <size_t size>
[[nodiscard]] int Tree<size>::get_tree_size() const {
  int sum = 0;
  int limit = size / 2;
  for (int i = 0; i < limit; ++i) {
    sum += 4;
  }
  return sum;
}

template <size_t size>
[[nodiscard]] inline int Tree<size>::get_compressed_size() const {
  return get_compressed_size(frequency);
}

template <size_t size>
[[nodiscard]] int Tree<size>::get_compressed_size(
  const std::array<int, size>& freq
) const {
  int sum = 0;
  int limit = size / 2;
  for (int i = 0; i < limit; ++i) {
    sum += freq[i] * bit_len[i];
  }
  return sum;
}

void Encoder::init() {
  output_buffer.clear();
  memset(&d_lc_tree, 0, sizeof(d_lc_tree));
  memset(&d_dist_tree, 0, sizeof(d_dist_tree));
}

void Encoder::count_frequency() {
  int size = static_cast<int>(lc_alphabet.size());
  for (int i = 0; i < size; ++i) {
    if (dist_alphabet[i]) {
      ++d_lc_tree.frequency[LEN_CODE_TABLE[lc_alphabet[i]] + MAX_CHAR + 1];
      ++d_dist_tree.frequency[get_dist_code(dist_alphabet[i])];
    } else {
      ++d_lc_tree.frequency[lc_alphabet[i]];
    }
  }
  ++d_lc_tree.frequency[256];  // EOF of a block
}

void Encoder::write_bits(uint32_t bits, int len, bool align) {
  static thread_local uint8_t bit_buf = 0;
  static thread_local uint8_t free_bit_len = 8;
  while (len >= free_bit_len) {
    bit_buf |= (bits >> (len - free_bit_len)) & ((1 << free_bit_len) - 1);
    len -= free_bit_len;
    output_buffer.push_back(bit_buf);
    bit_buf = 0;
    free_bit_len = 8;
  }
  bit_buf |= bits << (free_bit_len - len) & ((1 << free_bit_len) - 1);
  free_bit_len -= len;
  if (align && free_bit_len < 8) {
    output_buffer.push_back(bit_buf);
    bit_buf = 0;
    free_bit_len = 8;
  }
}

void Encoder::write_tree() {
  int limit = LC_CODE_COUNT;
  for (int i = 0; i < limit; ++i) {
    write_bits(d_lc_tree.bit_len[i], 4);
  }
  limit = DIST_CODE_COUNT;
  for (int i = 0; i < limit; ++i) {
    write_bits(d_dist_tree.bit_len[i], 4);
  }
}

void Encoder::compress(bool use_static) {
  write_bits(use_static, 1);  // write flag of if use_static
  const auto& lc_tree = use_static ? STATIC_LC_TREE : d_lc_tree;
  const auto& dist_tree = use_static ? STATIC_DIST_TREE : d_dist_tree;
  if (!use_static) {  // if use dynamic tree, write the tree
    write_tree();
  }

  int limit = static_cast<int>(lc_alphabet.size());
  for (int i = 0; i < limit; ++i) {
    if (dist_alphabet[i]) {
      uint16_t dist_code = get_dist_code(dist_alphabet[i]);
      write_bits(
        lc_tree.code[LEN_CODE_TABLE[lc_alphabet[i]] + MAX_CHAR + 1],
        lc_tree.bit_len[LEN_CODE_TABLE[lc_alphabet[i]] + MAX_CHAR + 1]
      );
      if (LEN_EXTRA_BITS[LEN_CODE_TABLE[lc_alphabet[i]]]) {
        write_bits(
          lc_alphabet[i] - LEN_BASE_TABLE[LEN_CODE_TABLE[lc_alphabet[i]]],
          LEN_EXTRA_BITS[LEN_CODE_TABLE[lc_alphabet[i]]]
        );
      }
      write_bits(dist_tree.code[dist_code], dist_tree.bit_len[dist_code]);
      if (DIST_EXTRA_BITS[dist_code]) {
        write_bits(
          dist_alphabet[i] - 1 - DIST_BASE_TABLE[dist_code],
          DIST_EXTRA_BITS[dist_code]
        );
      }
    } else {
      write_bits(lc_tree.code[lc_alphabet[i]], lc_tree.bit_len[lc_alphabet[i]]);
    }
  }
  write_bits(lc_tree.code[256], lc_tree.bit_len[256], true);  // write block eof
}

void Encoder::encode() {
  init();
  count_frequency();
  d_lc_tree.build_tree();
  d_dist_tree.build_tree();

  int static_size = STATIC_LC_TREE.get_compressed_size(d_lc_tree.frequency) +
                    STATIC_DIST_TREE.get_compressed_size(d_dist_tree.frequency);
  int dynamic_size =
    d_lc_tree.get_compressed_size() + d_dist_tree.get_compressed_size();
  int dynamic_tree_size =
    d_lc_tree.get_tree_size() + d_dist_tree.get_tree_size();

  compress(static_size <= dynamic_size + dynamic_tree_size);
}

namespace init {

constexpr decode_table GEN_STATIC_LC_DECODE_TABLE() {
  decode_table table{};
  uint16_t extra_count = 0;
  for (uint16_t i = 0; i < LC_CODE_COUNT; ++i) {
    if (STATIC_LC_TREE.bit_len[i] <= 8) {
      for (int j = STATIC_LC_TREE.code[i] << (8 - STATIC_LC_TREE.bit_len[i]);
           j < (STATIC_LC_TREE.code[i] + 1) << (8 - STATIC_LC_TREE.bit_len[i]);
           ++j) {
        table[j] = {
          i,
          static_cast<uint16_t>(
            STATIC_LC_TREE.code[i] << (16 - STATIC_LC_TREE.bit_len[i])
          ),
          STATIC_LC_TREE.bit_len[i]
        };
      }
    } else {
      if (!table[STATIC_LC_TREE.code[i] >> 1].letter) {
        table[STATIC_LC_TREE.code[i] >> 1] = {
          static_cast<uint16_t>(256 + extra_count), 0, STATIC_LC_TREE.bit_len[i]
        };
      }
      table[256 + extra_count] = {
        i,
        static_cast<uint16_t>(
          STATIC_LC_TREE.code[i] << (16 - STATIC_LC_TREE.bit_len[i])
        ),
        STATIC_LC_TREE.bit_len[i]
      };
      ++extra_count;
    }
  }
  return table;
}
constexpr decode_table STATIC_LC_DECODE_TABLE = GEN_STATIC_LC_DECODE_TABLE();

constexpr decode_table GEN_STATIC_DIST_DECODE_TABLE() {
  decode_table table{};
  for (int i = 0; i < DIST_CODE_COUNT; ++i) {
    for (int j = i << 3; j < (i + 1) << 3; ++j) {
      table[j] = {static_cast<uint16_t>(i), static_cast<uint16_t>(i << 11), 5};
    }
  }
  return table;
}
constexpr decode_table STATIC_DIST_DECODE_TABLE =
  GEN_STATIC_DIST_DECODE_TABLE();
}  // namespace init

using init::STATIC_DIST_DECODE_TABLE;  // static huffman tree decode table
using init::STATIC_LC_DECODE_TABLE;    // static huffman tree decode table

template <size_t size>
void gen_decode_table(decode_table& table, std::array<uint8_t, size>& bit_len) {
  using pii = std::pair<int, int>;
  std::priority_queue<pii, std::vector<pii>, std::greater<>> q;
  for (std::size_t i = 0; i < size; ++i) {
    if (bit_len[i]) {
      q.emplace(bit_len[i], i);
    }
  }

  // gen std huffman code
  std::array<int, MAX_BIT_LEN + 1> bit_len_count{};
  for (auto i : bit_len) {
    if (i > 0) {
      bit_len_count[i]++;
    }
  }
  std::array<uint16_t, size> code{};
  gen_huffman_code(code, bit_len, bit_len_count);

  uint16_t extra_count = 0;
  while (!q.empty()) {
    auto [len, letter] = q.top();
    q.pop();
    if (len <= 8) {
      for (int i = code[letter] << (8 - len);
           i < (code[letter] + 1) << (8 - len); ++i) {
        table[i] = {
          static_cast<uint16_t>(letter),
          static_cast<uint16_t>(code[letter] << (16 - len)),
          static_cast<uint8_t>(len)
        };
      }
    } else {
      if (!table[code[letter] >> (len - 8)].letter) {
        table[code[letter] >> (len - 8)] = {
          static_cast<uint16_t>(256 + extra_count), 0, static_cast<uint8_t>(len)
        };
      }
      table[256 + extra_count++] = {
        static_cast<uint16_t>(letter),
        static_cast<uint16_t>(code[letter] << (16 - len)),
        static_cast<uint8_t>(len)
      };
    }
  }
}

void try_move(
  uint8_t& data,
  uint16_t& buf,
  int& valid_data_len,
  int& valid_buf_len
) {
  if (valid_data_len) {
    if (valid_buf_len <= 8) {  // left shift
      buf |= static_cast<uint16_t>(data) << (8 - valid_buf_len);
      data = 0;
      valid_buf_len += valid_data_len;
      valid_data_len = 0;
    } else {  // right shift
      int moved_bit = std::min(16 - valid_buf_len, valid_data_len);
      buf |= data >> (valid_buf_len - 8);
      data <<= moved_bit;
      valid_buf_len += moved_bit;
      valid_data_len -= moved_bit;
    }
  }
  assert(valid_buf_len == 16 || valid_data_len == 0);
}

inline void throw_bits(uint16_t& buf, int& valid_buf_len, int len) {
  buf <<= len;
  valid_buf_len -= len;
  assert(valid_buf_len >= 0);
}

// return { matched_letter , used_bit_len }.
// if valid bit is not enough, character will be set UINT16_MAX
std::pair<uint16_t, uint8_t> look_decode_table(
  const decode_table& table,
  uint16_t bit_buf,
  int valid_buf_len
) {
  auto [letter, code, len] = table[bit_buf >> 8];
  assert(len > 0);
  if (len > valid_buf_len) {
    return {UINT16_MAX, len};
  }
  if (len <= 8) {
    return {letter, len};
  }
  static thread_local int last_pos = 0;
  for (int i = std::max(last_pos, static_cast<int>(letter));
       table[i].bit_len > 0; ++i) {
    auto [character, code, len] = table[i];
    if (len > valid_buf_len) {
      last_pos = i;
      return {UINT16_MAX, len};
    }
    if (code == (bit_buf & (UINT16_MAX << (16 - len)))) {
      last_pos = 0;
      return {character, len};
    }
  }
  assert(false);
  abort();  // the function should not run here.
}

void Decoder::init() {
  bit_buf = valid_buf_len = valid_data_len = lc_size = dist_size = 0;
  needed_bits = 1;
  lc_alphabet.clear();
  dist_alphabet.clear();
  dyn_lc_table.fill({});
  dyn_dist_table.fill({});
}

void Decoder::FSM_init() {
  bool use_dyn = !(bit_buf >> 15);
  status = use_dyn ? Status::BUILD_LC_TABLE : Status::DECODE_LC;
  throw_bits(bit_buf, valid_buf_len, 1);
  using_lc_table = use_dyn ? &dyn_lc_table : &STATIC_LC_DECODE_TABLE;
  using_dist_table = use_dyn ? &dyn_dist_table : &STATIC_DIST_DECODE_TABLE;
  needed_bits = use_dyn ? 4 : 7;
}

void Decoder::FSM_build_lc_table() {
  dyn_lc_bit_len[lc_size++] = bit_buf >> 12;
  throw_bits(bit_buf, valid_buf_len, 4);
  if (lc_size < LC_CODE_COUNT) {
    return;
  }
  gen_decode_table(dyn_lc_table, dyn_lc_bit_len);
  status = Status::BUILD_DIST_TABLE;
}

void Decoder::FSM_build_dist_table() {
  dyn_dist_bit_len[dist_size++] = bit_buf >> 12;
  throw_bits(bit_buf, valid_buf_len, 4);
  if (dist_size < DIST_CODE_COUNT) {
    return;
  }
  gen_decode_table(dyn_dist_table, dyn_dist_bit_len);
  status = Status::DECODE_LC;
  needed_bits = 1;
}

bool Decoder::FSM_decode_lc() {
  auto [lc_code, len] =
    look_decode_table(*using_lc_table, bit_buf, valid_buf_len);
  if (lc_code == UINT16_MAX) {
    needed_bits = len;
    return false;
  }
  throw_bits(bit_buf, valid_buf_len, len);
  needed_bits = 1;
  if (lc_code == 256) {
    valid_buf_len = 0;
    bit_buf = 0;
    valid_data_len = 0;
    status = Status::INIT;
    return true;
  }
  if (lc_code > MAX_CHAR) {
    lc_alphabet.push_back(LEN_BASE_TABLE[lc_code - MAX_CHAR - 1]);
    if (LEN_EXTRA_BITS[lc_code - MAX_CHAR - 1] > 0) {
      needed_bits = LEN_EXTRA_BITS[lc_code - MAX_CHAR - 1];
      status = Status::DECODE_LC_EXTRA;
      return false;
    }
    status = Status::DECODE_DIST;
    return false;
  }
  lc_alphabet.push_back(lc_code);
  dist_alphabet.push_back(0);
  return false;
}

void Decoder::FSM_decode_lc_extra() {
  lc_alphabet.back() += bit_buf >> (16 - needed_bits);
  throw_bits(bit_buf, valid_buf_len, needed_bits);
  needed_bits = 1;
  status = Status::DECODE_DIST;
}

void Decoder::FSM_decode_dist() {
  auto [dist_code, len] =
    look_decode_table(*using_dist_table, bit_buf, valid_buf_len);
  if (dist_code == UINT16_MAX) {
    needed_bits = len;
    return;
  }
  throw_bits(bit_buf, valid_buf_len, len);
  needed_bits = 1;
  dist_alphabet.push_back(DIST_BASE_TABLE[dist_code] + 1);
  if (DIST_EXTRA_BITS[dist_code] > 0) {
    needed_bits = DIST_EXTRA_BITS[dist_code];
    status = Status::DECODE_DIST_EXTRA;
    return;
  }
  status = Status::DECODE_LC;
}

void Decoder::FSM_decode_dist_extra() {
  dist_alphabet.back() += bit_buf >> (16 - needed_bits);
  throw_bits(bit_buf, valid_buf_len, needed_bits);
  needed_bits = 1;
  status = Status::DECODE_LC;
}

bool Decoder::decode(uint8_t data) {
  bool res = false;
  if (status == Status::INIT) {
    init();
  }
  valid_data_len = 8;
  while (valid_data_len + valid_buf_len >= needed_bits) {
    try_move(data, bit_buf, valid_data_len, valid_buf_len);
    switch (status) {
      case Status::INIT:
        FSM_init();
        break;
      case Status::BUILD_LC_TABLE:
        FSM_build_lc_table();
        break;
      case Status::BUILD_DIST_TABLE:
        FSM_build_dist_table();
        break;
      case Status::DECODE_LC:
        res = FSM_decode_lc();
        break;
      case Status::DECODE_LC_EXTRA:
        FSM_decode_lc_extra();
        break;
      case Status::DECODE_DIST:
        FSM_decode_dist();
        break;
      case Status::DECODE_DIST_EXTRA:
        FSM_decode_dist_extra();
        break;
    }
  }
  try_move(data, bit_buf, valid_data_len, valid_buf_len);
  assert(valid_data_len == 0);
  return res;
}

}  // namespace zipfiles::server::huffman