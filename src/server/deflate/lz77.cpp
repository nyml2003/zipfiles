#include <cstddef>
#include "server/deflate/lz77.h"

namespace zipfiles::server {

constexpr int WSIZE = 1 << 15;                            // window size
constexpr int MAX_MATCH = 258;                            // max match length
constexpr int MIN_MATCH = 3;                              // min match length
constexpr int MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1;  // min lookahead size
constexpr int MAX_DIST = WSIZE - MIN_LOOKAHEAD;  // max distance from match
constexpr int WINDOW_SIZE = WSIZE * 2;           // slide window size
constexpr int HTABLE_SIZE = 1 << 15;             // hash table size
constexpr int HSIZE = 1 << 15;                   // hash code count in bit
constexpr uint32_t HMASK = HSIZE - 1;            // hash code mask
constexpr int HSHIFT = 5;                        // bits of shift when cal hash
constexpr int NICE_MATCH = 128;                  // match length of stop search
constexpr int MAX_CHAIN = 128;  // max list length in hash table

inline uint32_t LZ77::next_hash(uint8_t byte) {
  return now_hash = ((now_hash << HSHIFT) ^ byte) & HMASK;
}

void LZ77::shift(int& hash_head) {
  next_hash(window[strstart + 2]);
  prev[strstart] = hash_head = head[now_hash];
  head[now_hash] = strstart;
}

int LZ77::fill_window() {
  int space = WINDOW_SIZE - lookahead - strstart;
  if (strstart >= WSIZE + MAX_DIST) {
    window += WSIZE;
    match_start -= WSIZE;
    strstart -= WSIZE;

    for (int i = 0; i < HSIZE; ++i) {
      head[i] = std::max(head[i] - WSIZE, 0);
    }
    for (int i = 0; i < WSIZE; ++i) {
      prev[i] = std::max(prev[i] - WSIZE, 0);
    }
    space += WSIZE;
  }

  int len = std::min(
    static_cast<int>(input_buffer_end - (window + strstart + lookahead)), space
  );
  eof = !len;
  lookahead += len;

  return len;
}

void LZ77::append(uint8_t literal_length, uint16_t distance) {
  lc_alphabet.push_back(literal_length);
  dist_alphabet.push_back(distance);
}

void LZ77::init(std::vector<uint8_t>& input_buffer) {
  strstart = 0;
  eof = false;

  head.assign(HTABLE_SIZE, 0);
  prev.resize(WINDOW_SIZE);
  input_buffer.resize(input_buffer.size() + MIN_LOOKAHEAD);
  input_buffer_end = input_buffer.end() - MIN_LOOKAHEAD;
  std::fill_n(input_buffer_end, MIN_LOOKAHEAD, 0);
  window = input_buffer.begin();
  lc_alphabet.clear();
  dist_alphabet.clear();

  lookahead =
    std::min(static_cast<int>(input_buffer_end - window), WINDOW_SIZE);
  if (lookahead == 0) {
    return;
  }
  while (lookahead < MIN_LOOKAHEAD && !eof && fill_window()) {
  }
  now_hash = 0;
  for (int i = 0; i < 2; ++i) {
    next_hash(window[i]);
  }
}

int LZ77::max_match(int cur_match) {
  int best_len = MIN_MATCH - 1;
  int len = 0;
  int chain_length = MAX_CHAIN;
  auto scan = window + strstart;
  auto match = scan;
  auto strend = scan + MAX_MATCH;
  int limit = std::max(strstart - MAX_DIST, 0);

  while (cur_match > limit && chain_length--) {
    match = window + cur_match - 1;
    scan = strend - MAX_MATCH - 1;

    while (*++scan == *++match && scan < strend) {
    }

    len = MAX_MATCH - static_cast<int>(strend - scan);

    if (len > best_len) {
      match_start = cur_match;
      best_len = len;
      if (len > NICE_MATCH) {
        break;
      }
    }
    cur_match = prev[cur_match];
  }

  return best_len;
}

int LZ77::encode(std::vector<uint8_t>& input_buffer) {
  int match_length = 0;
  int hash_head = 0;

  init(input_buffer);

  while (lookahead) {
    shift(hash_head);

    match_length = 0;
    if (hash_head && strstart - hash_head <= MAX_DIST && strstart <= WINDOW_SIZE - MIN_LOOKAHEAD) {
      match_length = std::min(max_match(hash_head), lookahead);
    }
    if (match_length < MIN_MATCH) {
      append(window[strstart], 0);
      strstart++;
      lookahead--;
    } else {
      append(match_length - MIN_MATCH, strstart - match_start);
      lookahead -= match_length;
      match_length--;
      while (match_length--) {
        strstart++;
        shift(hash_head);
      }
      strstart++;
    }

    while (lookahead < MIN_LOOKAHEAD && !eof && fill_window()) {
    }
  }
  return static_cast<int>(lc_alphabet.size());
}

void LZ77::decode(std::vector<uint8_t>& output_buffer) {
  output_buffer.clear();
  int length = 0;
  for (std::size_t i = 0; i < lc_alphabet.size(); ++i) {
    if (dist_alphabet[i]) {
      for (int j = 0; j < lc_alphabet[i] + MIN_MATCH; ++j) {
        output_buffer.push_back(output_buffer[length + j - dist_alphabet[i]]);
      }
      length += lc_alphabet[i] + MIN_MATCH;
    } else {
      output_buffer.push_back(lc_alphabet[i]);
      ++length;
    }
  }
}

}  // namespace zipfiles::server
