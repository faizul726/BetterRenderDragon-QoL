#pragma once
#include <string>
#include <utility>

namespace Core {

class Path;

template <typename Container> class PathBuffer;

using HeapPathBuffer = PathBuffer<std::string>;

class PathPart {
  std::string mUtf8StdString;

public:
  PathPart() = default;
  PathPart(std::string &&str) : mUtf8StdString(std::move(str)) {}
  PathPart(const std::string &str) : mUtf8StdString(str) {}
  // PathPart(gsl::not_null<const char*> pCStr) : mUtf8StdString(pCStr) {}
  // PathPart(gsl::not_null<const char*> pCStr, size_t size) :
  // mUtf8StdString(pCStr, size) {}
  PathPart(const char *pCStr) : mUtf8StdString(pCStr) {}
  PathPart(const char *pCStr, size_t size) : mUtf8StdString(pCStr, size) {}
  PathPart(const Core::Path &path);
  template <typename Container>
  PathPart(const Core::PathBuffer<Container> &pathBuffer);

  const char *getUtf8CString() const { return mUtf8StdString.c_str(); }
  const std::string &getUtf8StdString() const { return mUtf8StdString; }
  size_t size() const { return mUtf8StdString.size(); }
  bool empty() const { return mUtf8StdString.empty(); }
  bool operator<(const Core::PathPart &rhs) const {
    return mUtf8StdString < rhs.mUtf8StdString;
  }
  bool operator==(const Core::PathPart &rhs) const {
    return mUtf8StdString == rhs.mUtf8StdString;
  }
  bool operator!=(const Core::PathPart &rhs) const {
    return mUtf8StdString != rhs.mUtf8StdString;
  }
};

template <typename Container> class PathBuffer {
public:
  static const PathBuffer<Container> EMPTY;
  Container mContainer;

public:
  PathBuffer() = default;
  PathBuffer(Core::PathBuffer<Container> &&rhs) noexcept
      : mContainer(std::move(rhs.mContainer)) {}
  PathBuffer(const Core::PathBuffer<Container> &rhs)
      : mContainer(rhs.mContainer) {}
  PathBuffer(const std::string &s) : mContainer(s) {}
  PathBuffer(const char *s) : mContainer(s) {}
  // PathBuffer(gsl::string_span stringSpan);
  PathBuffer(const Core::Path &s);
  PathBuffer(Container &&container) : mContainer(std::move(container)) {}

  Core::PathBuffer<Container> &operator=(Core::PathBuffer<Container> &&rhs);
  Core::PathBuffer<Container> &operator=(const Core::Path &s);
  Core::PathBuffer<Container> &
  operator=(const Core::PathBuffer<Container> &rhs);
  Core::PathBuffer<Container> &operator+=(const char *str);
  bool operator==(const Core::Path &s) const;
  bool operator!=(const Core::Path &s) const;
  char const *getUtf8CString() const { return mContainer.c_str(); }
  // gsl::string_span getUtf8StringSpan() const;
  size_t size() const { return mContainer.size(); }
  void clear() { mContainer.clear(); }
  void reserve(size_t s) { mContainer.reserve(s); }
  bool empty() const { return mContainer.empty(); }
  void push_back(char c) { mContainer.push_back(c); }
  Container &&moveContainer();
  const Container &getContainer() const { return mContainer; }
};

class Path {
public:
  static const Core::Path EMPTY;
  Core::PathPart mPathPart;

public:
  Path() = default;
  Path(std::string &&str) : mPathPart(std::move(str)) {}
  Path(const std::string &str) : mPathPart(str) {}
  Path(const char *str) : mPathPart(str) {}
  Path(const char *str, size_t size) : mPathPart(str, size) {}
  Path(const Core::PathPart &a2) : mPathPart(a2) {}
  Path(const Core::Path &rhs) : mPathPart(rhs.mPathPart) {}
  Path(Core::Path &&path) noexcept : mPathPart(std::move(path.mPathPart)) {}

  template <typename Container>
  Path(const Core::PathBuffer<Container> &pathBuffer)
      : mPathPart(pathBuffer.getUtf8CString(), pathBuffer.size()) {}

  bool isDotOrDotDot() const;
  Core::Path &operator=(const Core::Path &other) {
    if (this != &other) {
      mPathPart = other.mPathPart;
    }
    return *this;
  }
  Core::Path &operator=(Core::Path &&other) noexcept {
    if (this != &other) {
      mPathPart = std::move(other.mPathPart);
    }
    return *this;
  }
  bool operator<(const Core::Path &other) const {
    return mPathPart < other.mPathPart;
  }
  bool operator==(const Core::Path &other) const {
    return mPathPart == other.mPathPart;
  }
  bool operator!=(const Core::Path &rhs) const;
  char const *getUtf8CString() const { return mPathPart.getUtf8CString(); }
  // gsl::string_span getUtf8StringSpan() const;
  const std::string &getUtf8StdString() const {
    return mPathPart.getUtf8StdString();
  }
  size_t size() const { return mPathPart.size(); }
  bool empty() const { return mPathPart.empty(); }
};

inline PathPart::PathPart(const Core::Path &path)
    : mUtf8StdString(path.getUtf8StdString()) {}

template <typename Container>
inline PathPart::PathPart(const Core::PathBuffer<Container> &pathBuffer)
    : mUtf8StdString(pathBuffer.getContainer()) {}

// ----- PathBuffer -----

template <typename Container>
inline PathBuffer<Container>::PathBuffer(const Core::Path &s)
    : mContainer(s.getUtf8StdString()) {}

template <typename Container>
inline Core::PathBuffer<Container> &
PathBuffer<Container>::operator=(Core::PathBuffer<Container> &&rhs) {
  if (this != &rhs) {
    mContainer = std::move(rhs.mContainer);
  }
  return *this;
}

template <typename Container>
inline Core::PathBuffer<Container> &
PathBuffer<Container>::operator=(const Core::Path &s) {
  mContainer = s.getUtf8StdString();
  return *this;
}

template <typename Container>
inline Core::PathBuffer<Container> &
PathBuffer<Container>::operator=(const Core::PathBuffer<Container> &rhs) {
  if (this != &rhs) {
    mContainer = rhs.mContainer;
  }
  return *this;
}

template <typename Container>
inline Core::PathBuffer<Container> &
PathBuffer<Container>::operator+=(const char *str) {
  mContainer += str;
  return *this;
}

template <typename Container>
inline bool PathBuffer<Container>::operator==(const Core::Path &s) const {
  return mContainer == s.getUtf8StdString();
}

template <typename Container>
inline bool PathBuffer<Container>::operator!=(const Core::Path &s) const {
  return !(*this == s);
}

template <typename Container>
inline Container &&PathBuffer<Container>::moveContainer() {
  return std::move(mContainer);
}

template <typename Container>
const PathBuffer<Container> PathBuffer<Container>::EMPTY =
    PathBuffer<Container>();

// ----- Path -----

inline bool Path::isDotOrDotDot() const {
  const std::string &s = mPathPart.getUtf8StdString();
  return (s == "." || s == "..");
}

inline bool Path::operator!=(const Core::Path &rhs) const {
  return !(*this == rhs);
}

// 静态成员定义
inline const Path Path::EMPTY = Path();

} // namespace Core