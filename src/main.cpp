#include <cstring>
#include <procmap/MemoryMap.hpp>

int main() {
  procmap::MemoryMap m;

  for (auto &segment : m) {
    if (segment.isWriteable() && segment.isReadable() &&
        !segment.isExecutable()) {
      auto p =
          malloc((char *)segment.endAddress() - (char *)segment.startAddress());
      memcpy(p, segment.startAddress(), segment.length());
      //   printf("%lx\n", p);

      //   segment.print();
    }
  }

  return 0;
}
