#ifndef ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H
#define ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include "server/deflate/huffman.h"
#include "server/deflate/lz77.h"

namespace zipfiles::server {

constexpr int ZIP_BLOCK_SIZE = 1 << 20;
constexpr int ZIP_INPUT_QUEUE_SIZE = 4;
constexpr int ZIP_WORKER_NUM = 4;

// data packet for zip input and processing
struct ZipDataPacket {
  int index = 0;  // packet index
  std::vector<uint8_t> data;

  // used by the priority queue
  bool operator<(const ZipDataPacket& other) const;
};

// data packet for zip output
struct ZippedDataPacket {
  bool valid = false;  // if the data is valid
  bool eof = false;    // if the data is the end of the file
  std::vector<uint8_t> data;
};

class Zip {
 private:
  // input queue
  int packet_index = 0;  // next packet index
  bool eof = false;      // end of input
  std::queue<ZipDataPacket> input_queue;
  std::mutex input_queue_mutex;
  std::condition_variable input_queue_cv;
  ZipDataPacket unfinished_input_packet;

  // output queue
  int output_index = 0;  // expected output index
  std::priority_queue<ZipDataPacket> output_queue;
  std::mutex output_queue_mutex;
  std::condition_variable output_queue_cv;

  // worker threads
  std::array<std::thread, ZIP_WORKER_NUM> worker_threads;

  // worker thread
  void worker();

 public:
  Zip() = default;
  Zip(const Zip&) = delete;
  Zip& operator=(const Zip&) = delete;
  Zip(Zip&&) = delete;
  Zip& operator=(Zip&&) = delete;
  ~Zip();

  /**
   * @brief
   * check if the input queue is full
   *
   * @return true
   * if the input queue is full
   *
   * @return false
   * if the input queue is not full
   */
  [[nodiscard]] bool full();

  /**
   * @brief
   * fill input to the input queue even if the input queue is full
   *
   * @param input
   * input data
   *
   * @param eof
   * if the input is the end of the file
   *
   * @return false
   * if the input queue is full
   */
  bool fill_input(const std::vector<uint8_t>& input, bool eof = false);

  /**
   * @brief
   * get the next output data.
   * you should call this function with block = true when eof is true
   *
   * @param block
   * if block is true, block until the next output is ready
   *
   * @return ZippedDataPacket
   * output data, look at the definition of ZippedDataPacket
   */
  ZippedDataPacket get_output(bool block = false);

  /**
   * @brief
   * initialize the worker threads.
   * you should call this function before calling fill_input and get_output.
   * you should call this function only once.
   * the worker threads will be terminated after you get all the output.
   */
  void init_worker();
};

// used by unzip
struct ZipBuffer {
  std::vector<uint8_t> ibuffer;         // input buffer
  std::vector<uint8_t> obuffer;         // output buffer
  std::vector<uint8_t> lc_alphabet;     // letter and length buffer
  std::vector<uint16_t> dist_alphabet;  // distance buffer
  ZipBuffer() {
    ibuffer.reserve(ZIP_BLOCK_SIZE + 5);
    obuffer.reserve(ZIP_BLOCK_SIZE + 5);
    lc_alphabet.reserve(ZIP_BLOCK_SIZE + 5);
    dist_alphabet.reserve(ZIP_BLOCK_SIZE + 5);
  }
};

// return status of unzip
struct ZipStatus {
  bool flush;                     // if there is output
  bool lack;                      // if there is not enough input
  std::vector<uint8_t>* obuffer;  // output data
};

class Unzip {
 private:
  const std::vector<uint8_t>* input = nullptr;
  std::vector<uint8_t>::const_iterator ibuf_start;

  ZipBuffer buffer;
  LZ77::Decoder lz77_decoder;
  huffman::Decoder huffman_decoder;

 public:
  Unzip()
    : lz77_decoder(buffer.lc_alphabet, buffer.dist_alphabet, buffer.obuffer),
      huffman_decoder(
        buffer.ibuffer,
        buffer.lc_alphabet,
        buffer.dist_alphabet
      ) {}
  void reset_input(const std::vector<uint8_t>* input_ptr);
  ZipStatus run();
};

}  // namespace zipfiles::server

#endif  // ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H