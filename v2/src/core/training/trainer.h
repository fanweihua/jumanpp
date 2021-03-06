//
// Created by Arseny Tolmachev on 2017/03/27.
//

#ifndef JUMANPP_TRAINER_H
#define JUMANPP_TRAINER_H

#include "core/analysis/perceptron.h"
#include "gold_example.h"
#include "loss.h"
#include "training_io.h"
#include "training_types.h"

namespace jumanpp {
namespace core {
namespace training {

class Trainer {
  analysis::AnalyzerImpl* analyzer;
  FullyAnnotatedExample example_;
  TrainingExampleAdapter adapter_;
  LossCalculator loss_;
  TrainingConfig config_;
  float currentLoss_;
  bool addedGoldNodes_;

 public:
  Trainer(analysis::AnalyzerImpl* analyzer, const spec::TrainingSpec* spec,
          const TrainingConfig& trconf)
      : analyzer{analyzer},
        adapter_{spec, analyzer},
        loss_{analyzer, spec},
        config_{trconf} {}

  void reset() {
    example_.reset();
    analyzer->reset();
    loss_.goldPath().reset();
    adapter_.reset();
    addedGoldNodes_ = false;
  }

  FullyAnnotatedExample& example() { return example_; }

  Status prepare();

  Status compute(const analysis::ScorerDef* sconf);

  void computeTrainingLoss();

  float lossValue() const { return currentLoss_; }

  util::ArraySlice<ScoredFeature> featureDiff() const {
    return loss_.featureDiff();
  }

  const GoldenPath& goldenPath() { return loss_.goldPath(); }

  bool wasGoldAdded() const { return addedGoldNodes_; }
};

struct TrainerFullConfig {
  const analysis::AnalyzerConfig& analyzerConfig;
  const CoreHolder& core;
  const spec::TrainingSpec& trainingSpec;
  const TrainingConfig& trainingConfig;
};

class OwningTrainer {
  analysis::AnalyzerImpl analyzer_;
  Trainer trainer_;
  bool wasPrepared = false;
  i64 lineNo_ = -1;

 public:
  explicit OwningTrainer(const TrainerFullConfig& conf)
      : analyzer_{&conf.core, ScoringConfig{conf.trainingConfig.beamSize, 1},
                  conf.analyzerConfig},
        trainer_{&analyzer_, &conf.trainingSpec, conf.trainingConfig} {}

  void reset() {
    wasPrepared = false;
    trainer_.reset();
  }

  Status initAnalyzer(const analysis::ScorerDef* sconf) {
    return analyzer_.initScorers(*sconf);
  }

  Status readExample(TrainingDataReader* rdr) {
    Status s = rdr->readFullExample(analyzer_.extraNodesContext(),
                                    &trainer_.example());
    lineNo_ = rdr->lineNumber();
    return s;
  }

  Status prepare() {
    if (wasPrepared) return Status::Ok();
    wasPrepared = true;
    return trainer_.prepare();
  }

  Status compute(const analysis::ScorerDef* sconf) {
    JPP_RETURN_IF_ERROR(trainer_.compute(sconf));
    trainer_.computeTrainingLoss();
    return Status::Ok();
  }

  float loss() const { return trainer_.lossValue(); }

  i64 line() const { return lineNo_; }

  util::ArraySlice<ScoredFeature> featureDiff() const {
    return trainer_.featureDiff();
  }

  analysis::AnalyzerImpl* anaImpl() { return &analyzer_; }

  Trainer* trainer() { return &trainer_; }
};

class BatchedTrainer {
  std::vector<std::unique_ptr<OwningTrainer>> trainers_;
  std::vector<i32> indices_;
  i32 current_;
  u32 seed_ = 0xdeadbeef;
  i32 numRead_ = 0;

 public:
  Status initialize(const TrainerFullConfig& tfc,
                    const analysis::ScorerDef* sconf, i32 numTrainers) {
    trainers_.clear();
    trainers_.reserve(numTrainers);
    for (int i = 0; i < numTrainers; ++i) {
      auto trainer = new OwningTrainer{tfc};
      JPP_RETURN_IF_ERROR(trainer->initAnalyzer(sconf));
      trainers_.emplace_back(trainer);
    }
    seed_ = tfc.trainingConfig.randomSeed;
    return Status::Ok();
  }

  Status readBatch(TrainingDataReader* rdr);

  OwningTrainer* trainer(i32 idx) const {
    JPP_DCHECK_EQ(activeTrainers(), indices_.size());
    JPP_DCHECK_IN(idx, 0, activeTrainers());
    auto idx2 = indices_[idx];
    JPP_DCHECK_IN(idx2, 0, activeTrainers());
    return trainers_[idx2].get();
  }

  i32 activeTrainers() const { return current_; }
};

}  // namespace training
}  // namespace core
}  // namespace jumanpp

#endif  // JUMANPP_TRAINER_H
