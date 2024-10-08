#ifndef ZIPFILES_INCLUDE_LZ77_H
#define ZIPFILES_INCLUDE_LZ77_H

#include <cstdint>
#include <vector>

namespace zipfiles::server::LZ77 {

class Encoder {
 private:
  std::vector<uint8_t>& ibuffer;  // input data buffer

  std::vector<uint8_t>& lc_alphabet;     // length or character alphabet
  std::vector<uint16_t>& dist_alphabet;  // distance alpabet

  std::vector<uint8_t>::iterator window;            // window head pointer
  std::vector<uint8_t>::iterator input_buffer_end;  // input buffer end pointer
  std::vector<int> head;                            // hash table list head
  std::vector<int> prev;  // hash table list prev pointer

  int strstart = 0;       // start index of lookahead window
  int match_start = 0;    // start index of matched str
  int lookahead = 0;      // lookahead length
  bool eof = false;       // eof flag of input buffer
  uint32_t now_hash = 0;  // now str(3 consecutive char) hash value

  // link three sequential byte
  uint32_t next_hash(uint8_t byte);
  // shift window 1 byte to the right
  void shift(int& hash_head);
  int fill_window();
  void append(uint8_t lc, uint16_t dist);
  void init();

  [[nodiscard]] int max_match(int cur_match);

 public:
  Encoder(
    std::vector<uint8_t>& ibuffer,
    std::vector<uint8_t>& literal_length_alphabet,
    std::vector<uint16_t>& distance_alphabet
  )
    : ibuffer(ibuffer),
      lc_alphabet(literal_length_alphabet),
      dist_alphabet(distance_alphabet) {};

  int run();
};

class Decoder {
 private:
  std::vector<uint8_t>& lc_alphabet;     // length or character alphabet
  std::vector<uint16_t>& dist_alphabet;  // distance alpabet

  std::vector<uint8_t>& obuffer;  // output data buffer

 public:
  Decoder(
    std::vector<uint8_t>& lc_alphabet,
    std::vector<uint16_t>& dist_alphabet,
    std::vector<uint8_t>& obuffer
  )
    : lc_alphabet(lc_alphabet),
      dist_alphabet(dist_alphabet),
      obuffer(obuffer) {};

  void run();
};
}  // namespace zipfiles::server::LZ77

#endif  // ZIPFILES_INCLUDE_LZ77_H