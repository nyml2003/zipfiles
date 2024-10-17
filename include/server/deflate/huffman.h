#ifndef ZIPFILES_INCLUDE_HUFFMAN_H
#define ZIPFILES_INCLUDE_HUFFMAN_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace zipfiles::server::huffman {

constexpr int LC_TREE_SIZE = 577;   // length or character huffman tree size
constexpr int DIST_TREE_SIZE = 61;  // distance huffman tree size
constexpr int DECODE_TABLE_SIZE = 1024;  // huffman decode table size
constexpr int MAX_BIT_LEN = 15;          // length or character max code length
constexpr int LEN_CODE_COUNT = 29;       // number of length code
constexpr int LC_CODE_COUNT = 287;       // number of length and character code
constexpr int DIST_CODE_COUNT = 30;      // number of distance code
constexpr int MAX_LEN = 256;             // max length - 3(min length)
constexpr int MAX_CHAR = 256;            // max character code
constexpr int DIST_CODE_TABLE_SIZE =
  512;  // length of the table of distance to distance code

// data stored as pair<code, bit_len>
struct decode_table_item {
  uint16_t letter;  // if code == 0, this will be search entry index in table
  uint16_t code;
  uint8_t bit_len;
};
using decode_table = std::array<decode_table_item, DECODE_TABLE_SIZE>;

template <size_t size>
struct Tree {
  std::array<int, size> frequency{};    // char/node frequency
  std::array<int, size> father{};       // father of the node
  std::array<uint8_t, size> bit_len{};  // bit length of huffman code
  std::array<uint16_t, size> code{};    // huffman code of the char

  // build huffman tree using the letter's frequency
  void build_tree();
  [[nodiscard]] int get_tree_size() const;
  [[nodiscard]] int get_compressed_size() const;
  [[nodiscard]] int get_compressed_size(const std::array<int, size>& freq
  ) const;
};

class Encoder {
 private:
  std::vector<uint8_t>& lc_alphabet;     // length or character alphabet
  std::vector<uint16_t>& dist_alphabet;  // distance alpabet

  std::vector<uint8_t>& obuffer;  // output buffer

  Tree<LC_TREE_SIZE>& d_lc_tree;      // dynamic length or character tree
  Tree<DIST_TREE_SIZE>& d_dist_tree;  // dynamic distance tree

  void init();
  void count_frequency();
  void write_bits(uint32_t bits, int len, bool align = false);
  void write_tree();
  void compress(bool use_static);

 public:
  Encoder(
    std::vector<uint8_t>& literal_length_alphabet,
    std::vector<uint16_t>& distance_alphabet,
    std::vector<uint8_t>& output_buffer
  )
    : lc_alphabet(literal_length_alphabet),
      dist_alphabet(distance_alphabet),
      obuffer(output_buffer),
      d_lc_tree(*new Tree<LC_TREE_SIZE>),
      d_dist_tree(*new Tree<DIST_TREE_SIZE>) {};

  ~Encoder() {
    delete &d_lc_tree;
    delete &d_dist_tree;
  }
  Encoder(Encoder&& other) = delete;
  Encoder(const Encoder& other) = delete;
  Encoder& operator=(Encoder&& other) = delete;
  Encoder& operator=(Encoder& other) = delete;

  void encode();
};

enum class Status {   // status machine status enum
  INIT,               // initial status
  BUILD_LC_TABLE,     // build lc decode table
  BUILD_DIST_TABLE,   // build dist decode table
  DECODE_LC,          // decode lc code
  DECODE_LC_EXTRA,    // decode lc extra code
  DECODE_DIST,        // decode dist code
  DECODE_DIST_EXTRA,  // decode dist extra code
};

class Decoder {
 private:
  std::vector<uint8_t>& ibuffer;  // input buffer

  std::vector<uint8_t>& lc_alphabet;     // length or character alphabet
  std::vector<uint16_t>& dist_alphabet;  // distance alpabet

  Status status = Status::INIT;                  // status code
  uint16_t bit_buf = 0;                          // store uncomplete bit code
  int valid_buf_len = 0;                         // free bit len in bit_buf
  std::vector<uint8_t>::iterator ibuffer_start;  // valid bit len in data
  std::vector<uint8_t>::iterator ibuffer_end;    // valid bit len in data
  int valid_data_len = 0;
  int needed_bits = 1;            // needed bits
  decode_table dyn_lc_table{};    // dynamic length and char decode table
  decode_table dyn_dist_table{};  // dynamic distance decode table
  const decode_table* using_lc_table =
    nullptr;  // using length and char decode table
  const decode_table* using_dist_table =
    nullptr;  // using distance decode table

  std::array<uint8_t, LC_CODE_COUNT> dyn_lc_bit_len{};
  int lc_size = 0;  // completed size of dyn_lc_bit_len
  std::array<uint8_t, DIST_CODE_COUNT> dyn_dist_bit_len{};
  int dist_size = 0;  // completed size of dyn_dist_bit_len

  bool fill_buf();

  void init();
  void FSM_init();
  void FSM_build_lc_table();
  void FSM_build_dist_table();
  bool FSM_decode_lc();
  void FSM_decode_lc_extra();
  void FSM_decode_dist();
  void FSM_decode_dist_extra();

 public:
  Decoder(
    std::vector<uint8_t>& input_buffer,
    std::vector<uint8_t>& lc_alphabet,
    std::vector<uint16_t>& dist_alphabet
  )
    : ibuffer(input_buffer),
      lc_alphabet(lc_alphabet),
      dist_alphabet(dist_alphabet) {};

  void reset_ibuf();
  bool decode();
};  // namespace decoder

}  // namespace zipfiles::server::huffman

#endif  // ZIPFILES_INCLUDE_HUFFMAN_H