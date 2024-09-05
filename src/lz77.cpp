#include "lz77.h"

namespace zipfiles::zip {
inline uint32_t LZ77::link_three_byte(int pos) {
  return ((uint8_t)input_buffer[pos] << 16) |
         ((uint8_t)input_buffer[pos + 1] << 8) | (uint8_t)input_buffer[pos + 2];
}

void LZ77::shift(int times) {
  uint32_t tail, head;
  while (times--) {
    tail = link_three_byte(wr - 2);
    hmap[tail].push_back(wr - 2);
    if (hmap[tail].size() > 2) {
      hmap[tail].pop_front();
    }
    ++wr;
    if (wr <= dict_size) {
      continue;
    }
    head = link_three_byte(wl);
    if (hmap[head].front() == wl) {
      hmap[head].pop_front();
    }
    if (hmap[head].empty()) {
      hmap.erase(head);
    }
    ++wl;
  }
}

inline void LZ77::append(uint16_t literal_length, uint16_t distance) {
  literal_length_alphabet.push_back(literal_length);
  distance_alphabet.push_back(distance);
}

inline void LZ77::init() {
  wr = wl = 0;
  hmap.clear();
  hmap[link_three_byte(0)].push_back(0);
  while (wr < 3) {
    append(input_buffer[wr], 0);
    ++wr;
  }
}

int LZ77::max_match(int pos) {
  int input_buffer_size = input_buffer.length();
  int length = 3;
  while (wr + length < input_buffer_size && length < max_match_length &&
         input_buffer[pos + length] == input_buffer[wr + length]) {
    ++length;
  }
  return length;
}

int LZ77::encode() {
  int input_buffer_size = input_buffer.length();
  if (input_buffer_size < 6) {
    return 2;
  }
  literal_length_alphabet.clear();
  distance_alphabet.clear();
  init();

  uint32_t now_st, max_length, max_match_location, now_max_match;
  while (wr < input_buffer_size) {
    if (input_buffer_size - wr < 3 || !hmap.count(now_st = link_three_byte(wr))) {
      append(input_buffer[wr], 0);
    } else {
      max_length = 0;
      for (int pos : hmap[now_st]) {
        now_max_match = max_match(pos);
        if (!max_length || now_max_match >= max_length) {
          max_match_location = pos;
          max_length = now_max_match;
        }
      }
      append(max_length, wr - max_match_location);
      shift(max_length - 1);
    }
    shift();
  }
  return input_buffer_size;
}

int LZ77::decode() {
  output_buffer.clear();
  int length = 0;
  for (int i = 0; i < literal_length_alphabet.size(); ++i) {
    if (distance_alphabet[i]) {
      for (int j = 0; j < literal_length_alphabet[i]; ++j) {
        output_buffer.push_back(output_buffer[length + j - distance_alphabet[i]]
        );
      }
      length += literal_length_alphabet[i];
    } else {
      output_buffer.push_back(literal_length_alphabet[i]);
      ++length;
    }
  }
  return length;
}

}  // namespace zipfiles::zip
