#include <cassert>
#include <cstdint>
#include <mutex>
#include <vector>

#include "server/deflate/huffman.h"
#include "server/deflate/lz77.h"
#include "server/deflate/zip.h"

namespace zipfiles::server {

bool ZipDataPacket::operator<(const ZipDataPacket& other) const {
  return index > other.index;
}

void Zip::worker() {
  std::unique_lock<std::mutex> input_lock(input_queue_mutex, std::defer_lock);

  while (true) {
    // wait for input
    input_lock.lock();
    input_queue_cv.wait(input_lock, [&] {
      return !input_queue.empty() || eof;
    });
    if (input_queue.empty() && eof) {
      input_lock.unlock();
      break;
    }

    // check if the input queue is empty and eof is false
    // this should never happen
    assert(input_queue.empty() == false);

    // get the input packet
    ZipDataPacket packet = std::move(input_queue.front());
    input_queue.pop();
    input_lock.unlock();

    // process the input packet
    std::vector<uint8_t> lc_alphabet;
    std::vector<uint16_t> dist_alphabet;
    LZ77::Encoder(packet.data, lc_alphabet, dist_alphabet).run();
    huffman::Encoder(lc_alphabet, dist_alphabet, packet.data).encode();

    // push the output packet into the output queue
    std::unique_lock<std::mutex> output_lock(output_queue_mutex);
    output_queue.push(std::move(packet));
    if (output_queue.top().index == output_index) {
      output_lock.unlock();
      output_queue_cv.notify_one();
    } else {
      output_lock.unlock();
    }
  }
}

Zip::~Zip() {
  for (std::thread& worker_thread : worker_threads) {
    if (worker_thread.joinable()) {
      worker_thread.join();
    }
  }
}

bool Zip::full() {
  std::lock_guard<std::mutex> input_lock(input_queue_mutex);
  std::lock_guard<std::mutex> output_lock(output_queue_mutex);
  return input_queue.size() >= ZIP_INPUT_QUEUE_SIZE || output_queue.size() > 1;
}

bool Zip::fill_input(const std::vector<uint8_t>& input, bool eof) {
  auto input_ptr = input.begin();
  while (input_ptr != input.end()) {
    // calculate the length of the data to be filled
    int len = std::min(
      static_cast<int>(ZIP_BLOCK_SIZE - unfinished_input_packet.data.size()),
      static_cast<int>(input.end() - input_ptr)
    );

    // fill the data
    unfinished_input_packet.data.insert(
      unfinished_input_packet.data.end(), input_ptr, input_ptr + len
    );

    // move the input pointer
    input_ptr += len;

    // if the packet is full, push it into the input queue and create a new
    // packet
    if (unfinished_input_packet.data.size() == ZIP_BLOCK_SIZE) {
      {
        std::lock_guard<std::mutex> lock(input_queue_mutex);
        input_queue.push(std::move(unfinished_input_packet));
        unfinished_input_packet = {++packet_index, {}};  // create a new packet
      }
      input_queue_cv.notify_one();
    }
  }

  // if eof is true and there is still data in the packet, push it into the
  // input queue
  if (eof) {
    std::unique_lock<std::mutex> lock(input_queue_mutex);
    this->eof = true;
    if (!unfinished_input_packet.data.empty()) {
      input_queue.push(std::move(unfinished_input_packet));
      unfinished_input_packet = {++packet_index, {}};  // create a new packet
      lock.unlock();
      input_queue_cv.notify_one();
    } else {
      lock.unlock();
    }
  }

  return full();
}

ZippedDataPacket Zip::get_output(bool block) {
  std::unique_lock<std::mutex> lock(output_queue_mutex);

  // if not blocking and the output queue is empty, return an empty packet
  if ((output_queue.empty() || output_queue.top().index != output_index) &&
      !block) {
    return {};
  }

  // wait for the output queue to be ready
  output_queue_cv.wait(lock, [&] {
    return !output_queue.empty() && output_queue.top().index == output_index;
  });

  // check if the next output is ready
  // this should never happen
  assert(output_queue.top().index == output_index);

  std::unique_lock<std::mutex> input_lock(input_queue_mutex);
  // get the output packet
  ZippedDataPacket packet = {
    true, eof && packet_index == output_index + 1,
    std::move(const_cast<ZipDataPacket&>(output_queue.top()).data)
  };
  output_queue.pop();
  output_index++;
  lock.unlock();

  // notify the worker thread to exit if the output queue is empty
  if (packet.eof) {
    input_lock.unlock();
    input_queue_cv.notify_all();
  } else {
    input_lock.unlock();
  }
  return packet;
}

void Zip::init_worker() {
  for (std::thread& worker_thread : worker_threads) {
    worker_thread = std::thread(&Zip::worker, this);
  }
}

void Unzip::reset_input(const std::vector<uint8_t>* input_ptr) {
  input = input_ptr;
  ibuf_start = input->begin();
}

ZipStatus Unzip::run() {
  auto reset_ibuffer = [&]() {
    buffer.ibuffer.clear();
    int len = std::min(
      static_cast<int>(ZIP_BLOCK_SIZE),
      static_cast<int>(input->end() - ibuf_start)
    );
    buffer.ibuffer.insert(buffer.ibuffer.end(), ibuf_start, ibuf_start + len);
    ibuf_start += len;
    huffman_decoder.reset_ibuf();
    assert(ibuf_start <= input->end());
  };

  if (ibuf_start == input->begin()) {
    reset_ibuffer();
  }

  while (!huffman_decoder.decode()) {
    if (ibuf_start == input->end()) {
      return {false, true, nullptr};
    }
    reset_ibuffer();
  }
  lz77_decoder.run();
  return {true, false, &buffer.obuffer};
}

}  // namespace zipfiles::server