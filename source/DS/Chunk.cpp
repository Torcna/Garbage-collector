#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>

#include "Base_ds/header_chunk.hpp"
#include "High_lvl/GC_B.hpp"

Chunk::Chunk(MemoryManager* memManager, size_t object_size, size_t chunk_size) {
  size_t header_size = sizeof(Header);
  size_t n = (chunk_size - header_size) / object_size;
  while (header_size + ((n + 7) / 8) + n * object_size > chunk_size && n > 0) {
    n--;
  }
  size_t num_objects = n;

  memory_ = reinterpret_cast<uint8_t*>(memManager->allocFromArena(chunk_size, 4096));
  header_ = reinterpret_cast<Header*>(memory_);
  header_->object_size_ = object_size;
  header_->num_objects_ = num_objects;
  header_->next_ = nullptr;
  size_t mark_bits_size = (num_objects + 7) / 8;
  mark_bits_ = memory_ + header_size;
  std::memset(mark_bits_, 0, mark_bits_size);

  data_ = mark_bits_ + mark_bits_size;

  std::cout << "Chunk layout:\n";
  std::cout << "  memory_   = " << static_cast<void*>(memory_) << "\n";
  std::cout << "  header_   = " << static_cast<void*>(header_) << " (size " << header_size << ")\n";
  std::cout << "  mark_bits_= " << static_cast<void*>(mark_bits_) << " (size " << mark_bits_size << ")\n";
  std::cout << "  data_     = " << static_cast<void*>(data_) << " (size " << num_objects * object_size << ")\n";
  std::cout << "  chunk_size= " << chunk_size << "\n";
  std::cout << "  num_objects in chunk = " << num_objects << "\n";
  std::cout << "  object_size in chunk = " << object_size << "\n";
}

size_t Chunk::getObjectSize() const { return header_->object_size_; }
size_t Chunk::getNumObjects() const { return header_->num_objects_; }

bool Chunk::isObjectMarked(size_t index) const {
  if (index >= header_->num_objects_) return false;
  return mark_bits_[index / 8] & (1 << (index % 8));
}

void Chunk::markObject(size_t index) {
  if (index < header_->num_objects_) mark_bits_[index / 8] |= (1 << (index % 8));
}

void Chunk::resetMarks() {
  size_t mark_bits_size = (header_->num_objects_ + 7) / 8;
  std::memset(mark_bits_, 0, mark_bits_size);
}

void* Chunk::getObject(size_t index) {
  return (index < header_->num_objects_) ? (data_ + index * header_->object_size_) : nullptr;
}

uint8_t* Chunk::getData() { return data_; }

Chunk* Chunk::getNext() const { return header_->next_; }
void Chunk::setNext(Chunk* next) { header_->next_ = next; }

void* Chunk::getStart() const { return memory_; }
size_t Chunk::getSize() const {
  return sizeof(Header) + ((header_->num_objects_ + 7) / 8) + header_->num_objects_ * header_->object_size_;
}