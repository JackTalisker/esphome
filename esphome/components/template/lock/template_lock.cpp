#include "template_lock.h"
#include "esphome/core/log.h"

namespace esphome::template_ {

using namespace esphome::lock;

static const char *const TAG = "template.lock";

TemplateLock::TemplateLock()
#ifdef USE_TEMPLATE_LOCK_LOCK_TRIGGER
    : lock_trigger_(new Trigger<>())
#ifdef USE_TEMPLATE_LOCK_UNLOCK_TRIGGER
      ,
      unlock_trigger_(new Trigger<>())
#endif
#ifdef USE_TEMPLATE_LOCK_OPEN_TRIGGER
      ,
      open_trigger_(new Trigger<>())
#endif
#elif defined(USE_TEMPLATE_LOCK_UNLOCK_TRIGGER)
    : unlock_trigger_(new Trigger<>())
#ifdef USE_TEMPLATE_LOCK_OPEN_TRIGGER
      ,
      open_trigger_(new Trigger<>())
#endif
#elif defined(USE_TEMPLATE_LOCK_OPEN_TRIGGER)
    : open_trigger_(new Trigger<>())
#endif
{
}

void TemplateLock::setup() {
  if (!this->f_.has_value())
    this->disable_loop();
}

void TemplateLock::loop() {
  auto val = this->f_();
  if (val.has_value()) {
    this->publish_state(*val);
  }
}
void TemplateLock::control(const lock::LockCall &call) {
#if defined(USE_TEMPLATE_LOCK_LOCK_TRIGGER) || defined(USE_TEMPLATE_LOCK_UNLOCK_TRIGGER) || \
    defined(USE_TEMPLATE_LOCK_OPEN_TRIGGER)
  if (this->prev_trigger_ != nullptr) {
    this->prev_trigger_->stop_action();
  }
#endif

  auto state = *call.get_state();
#ifdef USE_TEMPLATE_LOCK_LOCK_TRIGGER
  if (state == LOCK_STATE_LOCKED) {
    this->prev_trigger_ = this->lock_trigger_;
    this->lock_trigger_->trigger();
  }
#endif
#ifdef USE_TEMPLATE_LOCK_UNLOCK_TRIGGER
#ifdef USE_TEMPLATE_LOCK_LOCK_TRIGGER
  else
#endif
      if (state == LOCK_STATE_UNLOCKED) {
    this->prev_trigger_ = this->unlock_trigger_;
    this->unlock_trigger_->trigger();
  }
#endif

  if (this->optimistic_)
    this->publish_state(state);
}
void TemplateLock::open_latch() {
#ifdef USE_TEMPLATE_LOCK_OPEN_TRIGGER
#if defined(USE_TEMPLATE_LOCK_LOCK_TRIGGER) || defined(USE_TEMPLATE_LOCK_UNLOCK_TRIGGER)
  if (this->prev_trigger_ != nullptr) {
    this->prev_trigger_->stop_action();
  }
#endif
  this->prev_trigger_ = this->open_trigger_;
  this->open_trigger_->trigger();
#endif
}
void TemplateLock::set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }
float TemplateLock::get_setup_priority() const { return setup_priority::HARDWARE; }
#ifdef USE_TEMPLATE_LOCK_LOCK_TRIGGER
Trigger<> *TemplateLock::get_lock_trigger() const { return this->lock_trigger_; }
#endif
#ifdef USE_TEMPLATE_LOCK_UNLOCK_TRIGGER
Trigger<> *TemplateLock::get_unlock_trigger() const { return this->unlock_trigger_; }
#endif
#ifdef USE_TEMPLATE_LOCK_OPEN_TRIGGER
Trigger<> *TemplateLock::get_open_trigger() const { return this->open_trigger_; }
#endif
void TemplateLock::dump_config() {
  LOG_LOCK("", "Template Lock", this);
  ESP_LOGCONFIG(TAG, "  Optimistic: %s", YESNO(this->optimistic_));
}

}  // namespace esphome::template_
