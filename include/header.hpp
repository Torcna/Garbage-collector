#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>


class Chunk {
 private:
  struct Header {
    size_t object_size_; 
    size_t num_objects_;
    Chunk* next_;        
  };

  Header* header_;      // header right in the beggining of allocated chunk
  uint8_t* mark_bits_;  // ”казатель на массив меток
  uint8_t* data_;       // ”казатель на начало пам€ти дл€ объектов

 public:
  explicit Chunk(size_t object_size, size_t chunk_size = 4096) {
    size_t header_size = sizeof(Header);


    size_t num_objects = (chunk_size - header_size) / (object_size + 1);

    uint8_t* raw_memory = static_cast<uint8_t*>(std::malloc(chunk_size));
    if (!raw_memory) {
      throw std::bad_alloc();
    }


    header_ = reinterpret_cast<Header*>(raw_memory);
    header_->object_size_ = object_size;
    header_->num_objects_ = num_objects;
    header_->next_ = nullptr;

    mark_bits_ = raw_memory + header_size;
    std::memset(mark_bits_, 0, num_objects);

    data_ = mark_bits_ + num_objects;
  }

  ~Chunk() { std::free(header_); }

  size_t getObjectSize() const { return header_->object_size_; }
  size_t getNumObjects() const { return header_->num_objects_; }

  void* getObject(size_t index) {
    if (index >= header_->num_objects_) return nullptr;
    return data_ + index * header_->object_size_;
  }

  bool isObjectMarked(size_t index) const { return index < header_->num_objects_ && mark_bits_[index] != 0; }

  void markObject(size_t index) {
    if (index < header_->num_objects_) {
      mark_bits_[index] = 1;
    }
  }

  void resetMarks() { std::memset(mark_bits_, 0, header_->num_objects_); }

  uint8_t* get_data() { return data_; }

  Chunk* getNext() const { return header_->next_; }
  void setNext(Chunk* next) { header_->next_ = next; }
};


class FreeList {
 private:
  struct FreeBlock {
    FreeBlock* next;
  };
  FreeBlock* head_;

 public:
  FreeList() : head_(nullptr) {}

  void* allocate() {
    if (!head_) return nullptr;
    FreeBlock* block = head_;
    head_ = head_->next;
    return block;
  }

  void deallocate(void* ptr) {
    FreeBlock* block = static_cast<FreeBlock*>(ptr);
    block->next = head_;
    head_ = block;
  }

  bool isEmpty() const { return head_ == nullptr; }

  void addBlock(void* ptr) {
    FreeBlock* block = static_cast<FreeBlock*>(ptr);
    block->next = head_;
    head_ = block;
  }
};




class Allocator {
 private:
  std::map<size_t, std::vector<Chunk*>> chunk_pools_;
  std::map<size_t, FreeList> free_lists_;

 public:
  Allocator() = default;

  ~Allocator() {
    for (auto& [size, chunks] : chunk_pools_) {
      for (Chunk* chunk : chunks) {
        delete chunk;
      }
    }
    chunk_pools_.clear();
    free_lists_.clear();
  }

  template <typename T>
  void* allocate() {
    size_t object_size = sizeof(T);

    if (free_lists_[object_size].isEmpty()) {
      allocateNewChunk(object_size);
    }

    return free_lists_[object_size].allocate();
  }

  template <typename T>
  void deallocate(void* ptr) {
    size_t object_size = sizeof(T);
    free_lists_[object_size].deallocate(ptr);
  }

  void resetMarks() {
    for (auto& [size, chunks] : chunk_pools_) {
      for (Chunk* chunk : chunks) {
        chunk->resetMarks();
      }
    }
  }

  void mark(void* ptr) {
    for (auto& [size, chunks] : chunk_pools_) {
      for (Chunk* chunk : chunks) {
        if (ptr >= chunk->get_data() && ptr < (chunk->get_data() + chunk->getObjectSize() * chunk->getNumObjects())) {
          size_t index = (static_cast<uint8_t*>(ptr) - chunk->get_data()) / chunk->getObjectSize();
          chunk->markObject(index);
          return;
        }
      }
    }
  }

  void collect() {
    for (auto& [size, chunks] : chunk_pools_) {
      for (Chunk* chunk : chunks) {
        for (size_t i = 0; i < chunk->getNumObjects(); ++i) {
          if (!chunk->isObjectMarked(i)) {
            void* object = chunk->getObject(i);
            free_lists_[size].addBlock(object);
          }
        }
      }
    }
    resetMarks();
  }

 private:
  void allocateNewChunk(size_t object_size) {
    Chunk* new_chunk = new Chunk(object_size);
    chunk_pools_[object_size].push_back(new_chunk);

    for (size_t i = 0; i < new_chunk->getNumObjects(); ++i) {
      void* object = new_chunk->getObject(i);
      free_lists_[object_size].addBlock(object);
    }
  }
};



