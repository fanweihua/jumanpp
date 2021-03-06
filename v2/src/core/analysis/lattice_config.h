//
// Created by Arseny Tolmachev on 2017/03/02.
//

#ifndef JUMANPP_LATTICE_CONFIG_H
#define JUMANPP_LATTICE_CONFIG_H

#include <functional>
#include "util/types.hpp"

namespace jumanpp {
namespace core {
namespace analysis {

class LatticePlugin;

using LatticePosition = u16;
using Score = float;

struct alignas(alignof(u32)) LatticeNodePtr {
  u16 boundary;
  u16 position;
};

inline bool operator==(const LatticeNodePtr& p1, const LatticeNodePtr& p2) {
  return p1.boundary == p2.boundary && p1.position == p2.position;
}

struct alignas(alignof(u64)) ConnectionPtr {
  // boundary where the connection was created
  u16 boundary;
  // offset of ending node inside the boundary (get the actual pointer from
  // boundary.ends[left])
  u16 left;
  // offset of starting node inside the boundary
  u16 right;
  // beam index for getting additional context
  // the context is stored in the current boundary
  u16 beam;

  // pointer to the previous entry
  const ConnectionPtr* previous;

  LatticeNodePtr latticeNodePtr() const {
    return LatticeNodePtr{boundary, right};
  }
};

inline bool operator==(const ConnectionPtr& p1, const ConnectionPtr& p2) {
  return p1.boundary == p2.boundary && p1.left == p2.left &&
         p1.right == p2.right && p1.beam == p2.beam &&
         p1.previous == p2.previous;
}

struct LatticeConfig {
  u32 entrySize;
  u32 numPrimitiveFeatures;
  u32 numFeaturePatterns;
  u32 numFinalFeatures;
  u32 beamSize;
  u32 scoreCnt;
};

struct LatticeBoundaryConfig {
  u32 boundary;
  u32 endNodes;
  u32 beginNodes;
};

struct ConnectionBeamElement {
  ConnectionPtr ptr;
  Score totalScore;
};

class Lattice;

}  // namespace analysis
}  // namespace core
}  // namespace jumanpp

namespace std {

template <>
struct hash<jumanpp::core::analysis::LatticeNodePtr> {
  using argument_type = jumanpp::core::analysis::LatticeNodePtr;
  using result_type = size_t;
  size_t operator()(jumanpp::core::analysis::LatticeNodePtr ptr) const {
    using jumanpp::u64;
    using jumanpp::u8;
    u64 result = 0;
    result += static_cast<u64>(ptr.boundary) * 0xda12'1512'fa23'245cULL;
    result += static_cast<u64>(ptr.position) * 0x1251'2321'5191'fa99ULL;
    u8 shift = static_cast<u8>(result >> 59);
    return static_cast<size_t>(result ^ (result >> shift));
  }
};

template <>
struct hash<jumanpp::core::analysis::ConnectionPtr> {
  using argument_type = jumanpp::core::analysis::ConnectionPtr;
  using result_type = size_t;
  result_type operator()(
      jumanpp::core::analysis::ConnectionPtr const& ptr) const {
    using jumanpp::u64;
    using jumanpp::u8;
    u64 result = 0;
    result += static_cast<u64>(ptr.boundary) * 0xda12'1818'fa23'245cULL;
    result += static_cast<u64>(ptr.left) * 0x1251'2512'5191'fa99ULL;
    result += static_cast<u64>(ptr.right) * 0x7612'51fa'6122'9887ULL;
    result += static_cast<u64>(ptr.beam) * 0x9212'2321'af22'246fULL;
    result += reinterpret_cast<u64>(ptr.previous) * 0x4512'1221'5555'4121ULL;
    u8 shift = static_cast<u8>(result >> 59);
    return static_cast<size_t>(result ^ (result >> shift));
  }
};

}  // namespace std

#endif  // JUMANPP_LATTICE_CONFIG_H
