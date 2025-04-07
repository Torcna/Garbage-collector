#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

class Chunk {
 private:
  struct Header {
    size_t object_size_;
    size_t num_objects_;
    Chunk* next_;
  };

  std::unique_ptr<uint8_t[]> memory_;
  Header* header_;
  uint8_t* mark_bits_;
  uint8_t* data_;

 public:
  explicit Chunk(size_t object_size, size_t chunk_size = 4096);

  size_t getObjectSize() const;
  size_t getNumObjects() const;

  bool isObjectMarked(size_t index) const;
  void markObject(size_t index);
  void resetMarks();

  void* getObject(size_t index);
  uint8_t* getData();

  Chunk* getNext() const;
  void setNext(Chunk* next);

  void* getStart() const;
  size_t getSize() const;
};