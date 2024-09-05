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
  std::string& input_buffer;
  std::vector<uint16_t>& literal_length_alphabet;
  std::vector<uint16_t>& distance_alphabet;
  std::string& output_buffer;
  std::unordered_map<uint32_t, std::list<uint32_t>> hmap;

  static const int dict_size = 1 << 15;
  static const int max_match_length = 258;
  int wr, wl;  // dict_window: [wl, wr)

  // link three sequential byte
  uint32_t link_three_byte(int pos);
  // shift window 1 byte to the right
  void shift(int times = 1);

  void append(uint16_t a, uint16_t b);
  void init();

  int max_match(int pos);

 public:
  LZ77(
    std::string& input_buffer,
    std::vector<uint16_t>& literal_length_alphabet,
    std::vector<uint16_t>& distance_alphabet,
    std::string& output_buffer
  )
    : input_buffer(input_buffer),
      literal_length_alphabet(literal_length_alphabet),
      distance_alphabet(distance_alphabet),
      output_buffer(output_buffer),
      wr(0),
      wl(0){};

  // Return Code: 2:do nothing
  int encode();

  int decode();
};
}  // namespace zipfiles::zip

#endif  // ZIPFILES_INCLUDE_LZ77_H