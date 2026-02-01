#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/template_lambda.h"
#include "esphome/components/lock/lock.h"

namespace esphome::template_ {

class TemplateLock final : public lock::Lock, public Component {
 public:
  TemplateLock();

  void setup() override;
  void dump_config() override;

  template<typename F> void set_state_lambda(F &&f) { this->f_.set(std::forward<F>(f)); }
#ifdef USE_TEMPLATE_LOCK_LOCK_TRIGGER
  Trigger<> *get_lock_trigger() const;
#endif
#ifdef USE_TEMPLATE_LOCK_UNLOCK_TRIGGER
  Trigger<> *get_unlock_trigger() const;
#endif
#ifdef USE_TEMPLATE_LOCK_OPEN_TRIGGER
  Trigger<> *get_open_trigger() const;
#endif
  void set_optimistic(bool optimistic);
  void loop() override;

  float get_setup_priority() const override;

 protected:
  void control(const lock::LockCall &call) override;
  void open_latch() override;

  TemplateLambda<lock::LockState> f_;
  bool optimistic_{false};
#ifdef USE_TEMPLATE_LOCK_LOCK_TRIGGER
  Trigger<> *lock_trigger_;
#endif
#ifdef USE_TEMPLATE_LOCK_UNLOCK_TRIGGER
  Trigger<> *unlock_trigger_;
#endif
#ifdef USE_TEMPLATE_LOCK_OPEN_TRIGGER
  Trigger<> *open_trigger_;
#endif
#if defined(USE_TEMPLATE_LOCK_LOCK_TRIGGER) || defined(USE_TEMPLATE_LOCK_UNLOCK_TRIGGER) || \
    defined(USE_TEMPLATE_LOCK_OPEN_TRIGGER)
  Trigger<> *prev_trigger_{nullptr};
#endif
};

}  // namespace esphome::template_
