#ifndef ZIPFILES_INCLUDE_LZ77_H
#define ZIPFILES_INCLUDE_LZ77_H

#include <cstdint>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace zipfiles::zip {

class LZ77 {
 private:
  static constexpr int WSIZE = 1 << 15;
  static constexpr int MAX_MATCH = 258;
  static constexpr int MIN_MATCH = 3;
  static constexpr int MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1;
  static constexpr int MAX_DIST = WSIZE - MIN_LOOKAHEAD;
  static constexpr int WINDOW_SIZE = WSIZE * 2;
  static constexpr int HTABLE_SIZE = 1 << 15;
  static constexpr int HSIZE = 1 << 15;
  static constexpr uint32_t HMASK = HSIZE - 1;
  static constexpr int HSHIFT = 5;
  static constexpr int NICE_MATCH = 128;
  static constexpr int MAX_CHAIN = 128;

  std::vector<uint8_t>& input_buffer;
  std::vector<uint8_t>& literal_length_alphabet;
  std::vector<uint16_t>& distance_alphabet;
  std::vector<uint8_t>& output_buffer;
  std::vector<uint8_t>::iterator window, input_buffer_end;
  std::vector<int> head;
  std::vector<int> prev;

  int strstart, match_start, lookahead, input_size;
  bool eof;
  uint32_t now_hash;

  // link three sequential byte
  uint32_t next_hash(uint8_t byte);
  // shift window 1 byte to the right
  void shift(int& hash_head);
  int fill_window();
  void append(uint8_t a, uint16_t b);
  void init();

  int max_match(int hash_head);

 public:
  LZ77(
    std::vector<uint8_t>& input_buffer,
    std::vector<uint8_t>& literal_length_alphabet,
    std::vector<uint16_t>& distance_alphabet,
    std::vector<uint8_t>& output_buffer
  )
    : input_buffer(input_buffer),
      literal_length_alphabet(literal_length_alphabet),
      distance_alphabet(distance_alphabet),
      output_buffer(output_buffer){};

  // Return Code: 2:do nothing
  int encode();

  int decode();
};
}  // namespace zipfiles::zip

#endif  // ZIPFILES_INCLUDE_LZ77_H