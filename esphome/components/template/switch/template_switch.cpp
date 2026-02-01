#include "template_switch.h"
#include "esphome/core/log.h"

namespace esphome::template_ {

static const char *const TAG = "template.switch";

TemplateSwitch::TemplateSwitch()
#ifdef USE_TEMPLATE_SWITCH_TURN_ON_TRIGGER
    : turn_on_trigger_(new Trigger<>())
#ifdef USE_TEMPLATE_SWITCH_TURN_OFF_TRIGGER
      ,
      turn_off_trigger_(new Trigger<>())
#endif
#elif defined(USE_TEMPLATE_SWITCH_TURN_OFF_TRIGGER)
    : turn_off_trigger_(new Trigger<>())
#endif
{
}

void TemplateSwitch::loop() {
  auto s = this->f_();
  if (s.has_value()) {
    this->publish_state(*s);
  }
}
void TemplateSwitch::write_state(bool state) {
#if defined(USE_TEMPLATE_SWITCH_TURN_ON_TRIGGER) || defined(USE_TEMPLATE_SWITCH_TURN_OFF_TRIGGER)
  if (this->prev_trigger_ != nullptr) {
    this->prev_trigger_->stop_action();
  }
#endif

  if (state) {
#ifdef USE_TEMPLATE_SWITCH_TURN_ON_TRIGGER
    this->prev_trigger_ = this->turn_on_trigger_;
    this->turn_on_trigger_->trigger();
#endif
  } else {
#ifdef USE_TEMPLATE_SWITCH_TURN_OFF_TRIGGER
    this->prev_trigger_ = this->turn_off_trigger_;
    this->turn_off_trigger_->trigger();
#endif
  }

  if (this->optimistic_)
    this->publish_state(state);
}
void TemplateSwitch::set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }
bool TemplateSwitch::assumed_state() { return this->assumed_state_; }
float TemplateSwitch::get_setup_priority() const { return setup_priority::HARDWARE - 2.0f; }
#ifdef USE_TEMPLATE_SWITCH_TURN_ON_TRIGGER
Trigger<> *TemplateSwitch::get_turn_on_trigger() const { return this->turn_on_trigger_; }
#endif
#ifdef USE_TEMPLATE_SWITCH_TURN_OFF_TRIGGER
Trigger<> *TemplateSwitch::get_turn_off_trigger() const { return this->turn_off_trigger_; }
#endif
void TemplateSwitch::setup() {
  if (!this->f_.has_value())
    this->disable_loop();

  optional<bool> initial_state = this->get_initial_state_with_restore_mode();

  if (initial_state.has_value()) {
    ESP_LOGD(TAG, "  Restored state %s", ONOFF(initial_state.value()));
    // if it has a value, restore_mode is not "DISABLED", therefore act on the switch:
    if (initial_state.value()) {
      this->turn_on();
    } else {
      this->turn_off();
    }
  }
}
void TemplateSwitch::dump_config() {
  LOG_SWITCH("", "Template Switch", this);
  ESP_LOGCONFIG(TAG, "  Optimistic: %s", YESNO(this->optimistic_));
}
void TemplateSwitch::set_assumed_state(bool assumed_state) { this->assumed_state_ = assumed_state; }

}  // namespace esphome::template_
