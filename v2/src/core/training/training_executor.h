//
// Created by Arseny Tolmachev on 2017/04/06.
//

#ifndef JUMANPP_TRAINING_EXECUTOR_H
#define JUMANPP_TRAINING_EXECUTOR_H

#include "scw.h"
#include "trainer.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace jumanpp {
namespace core {
namespace training {

struct TrainingExecutionResult {
  OwningTrainer* trainer;
  Status processStatus;
};

enum class ExecutorThreadState {
  Initialization,
  WaitingForInput,
  HaveInput,
  RunningComputation,
  ComputationFinished,
  Exiting
};

class TrainingExecutorThread {
  const analysis::ScorerDef* scoreConf_;
  Status processStatus_;
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable input_ready_;
  OwningTrainer* trainer_;
  std::atomic<ExecutorThreadState> state_{ExecutorThreadState::Initialization};

  void run();

  // an entry poinf the the thread
  static void runMain(TrainingExecutorThread* ctx) { ctx->run(); }

 public:
  explicit TrainingExecutorThread(const analysis::ScorerDef* conf);
  void publishTrainer(OwningTrainer* trainer);
  TrainingExecutionResult waitForTrainer();
  ~TrainingExecutorThread();
};

class TrainingExecutor {
  std::vector<std::unique_ptr<TrainingExecutorThread>> threads_;
  u32 head_;
  u32 tail_;

 public:
  Status initialize(const analysis::ScorerDef* sconf, u32 nthreads);

  u32 capacity() const { return (u32)threads_.size(); }

  u32 used() const { return head_ - tail_; }

  u32 available() const { return capacity() - used(); }

  bool nonProcessedExist() const { return head_ != tail_; }

  bool runNext(OwningTrainer* next, TrainingExecutionResult* result);

  TrainingExecutionResult waitOne();
};

}  // namespace training
}  // namespace core
}  // namespace jumanpp

#endif  // JUMANPP_TRAINING_EXECUTOR_H
