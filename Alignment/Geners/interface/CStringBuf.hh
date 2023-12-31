#ifndef GENERS_CSTRINGBUF_HH_
#define GENERS_CSTRINGBUF_HH_

#include <sstream>

namespace gs {
  class CStringBuf : public std::stringbuf {
  public:
    explicit CStringBuf(std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) : std::stringbuf(mode) {}

    const char *getGetBuffer(unsigned long long *len) const;
    const char *getPutBuffer(unsigned long long *len) const;

    CStringBuf(const CStringBuf &) = delete;
    CStringBuf &operator=(const CStringBuf &) = delete;
  };
}  // namespace gs

#endif  // GENERS_CSTRINGBUF_HH_
