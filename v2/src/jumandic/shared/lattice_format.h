//
// Created by Arseny Tolmachev on 2017/07/21.
//

#ifndef JUMANPP_LATTICE_FORMAT_H
#define JUMANPP_LATTICE_FORMAT_H

#include "core/analysis/lattice_config.h"
#include "juman_format.h"

namespace jumanpp {
namespace jumandic {
namespace output {

using core::analysis::LatticeNodePtr;

struct LatticeNodeInfo {
  util::InlinedVector<u16, 32> ranks;
  util::FlatSet<core::analysis::ConnectionPtr> ptrs;

  void addElem(const core::analysis::ConnectionPtr& cptr, i32 rank) {
    ranks.push_back(static_cast<u16>(rank));
    ptrs.insert(cptr);
  }
};

class LatticeInfoView {
  LatticeNodePtr nodePtr_;
  const LatticeNodeInfo* node_;

  LatticeInfoView(const LatticeNodePtr& nodePtr_, LatticeNodeInfo* node_)
      : nodePtr_(nodePtr_), node_(node_) {}

 public:
  const LatticeNodePtr& nodePtr() const { return nodePtr_; }
  const LatticeNodeInfo& nodeInfo() const { return *node_; }

  friend class LatticeFormatInfo;
};

class LatticeFormatInfo {
  util::FlatMap<LatticeNodePtr, LatticeNodeInfo> info;

 public:
  Status fillInfo(const core::analysis::Analyzer& an, i32 topN);
  void publishResult(std::vector<LatticeInfoView>* view);
};

class LatticeFormat : public OutputFormat {
  JumandicFields flds;
  util::io::Printer printer;
  std::array<i32, JumandicNumFields> fieldBuffer;
  core::analysis::NodeWalker walker{&fieldBuffer};
  JumandicIdResolver idResolver;
  LatticeFormatInfo latticeInfo;
  util::FlatMap<core::analysis::LatticeNodePtr, std::string> prevIds;
  i32 currentId;

  i32 topN;
  std::vector<LatticeInfoView> infoView;

  i32 appendId(core::analysis::LatticeNodePtr cptr);
  StringPiece prevIdFor(core::analysis::LatticeNodePtr ptr) const;

 public:
  LatticeFormat(i32 topN) : topN(topN) {}
  Status initialize(const core::analysis::OutputManager& om);
  Status format(const core::analysis::Analyzer& analyzer,
                StringPiece comment) override;
  StringPiece result() const override;
};

}  // namespace output
}  // namespace jumandic
}  // namespace jumanpp

#endif  // JUMANPP_LATTICE_FORMAT_H
