#include "template_valve.h"
#include "esphome/core/log.h"

namespace esphome::template_ {

using namespace esphome::valve;

static const char *const TAG = "template.valve";

TemplateValve::TemplateValve()
    : open_trigger_(new Trigger<>()),
      close_trigger_(new Trigger<>())
#ifdef USE_TEMPLATE_VALVE_STOP_TRIGGER
      ,
      stop_trigger_(new Trigger<>())
#endif
#ifdef USE_TEMPLATE_VALVE_TOGGLE_TRIGGER
      ,
      toggle_trigger_(new Trigger<>())
#endif
#ifdef USE_TEMPLATE_VALVE_POSITION_TRIGGER
      ,
      position_trigger_(new Trigger<float>())
#endif
{
}

void TemplateValve::setup() {
  switch (this->restore_mode_) {
    case VALVE_NO_RESTORE:
      break;
    case VALVE_RESTORE: {
      auto restore = this->restore_state_();
      if (restore.has_value())
        restore->apply(this);
      break;
    }
    case VALVE_RESTORE_AND_CALL: {
      auto restore = this->restore_state_();
      if (restore.has_value()) {
        restore->to_call(this).perform();
      }
      break;
    }
  }
  if (!this->state_f_.has_value())
    this->disable_loop();
}

void TemplateValve::loop() {
  bool changed = false;

  auto s = this->state_f_();
  if (s.has_value()) {
    auto pos = clamp(*s, 0.0f, 1.0f);
    if (pos != this->position) {
      this->position = pos;
      changed = true;
    }
  }

  if (changed)
    this->publish_state();
}

void TemplateValve::set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }
void TemplateValve::set_assumed_state(bool assumed_state) { this->assumed_state_ = assumed_state; }
float TemplateValve::get_setup_priority() const { return setup_priority::HARDWARE; }

Trigger<> *TemplateValve::get_open_trigger() const { return this->open_trigger_; }
Trigger<> *TemplateValve::get_close_trigger() const { return this->close_trigger_; }
#ifdef USE_TEMPLATE_VALVE_STOP_TRIGGER
Trigger<> *TemplateValve::get_stop_trigger() const { return this->stop_trigger_; }
#endif
#ifdef USE_TEMPLATE_VALVE_TOGGLE_TRIGGER
Trigger<> *TemplateValve::get_toggle_trigger() const { return this->toggle_trigger_; }
#endif

void TemplateValve::dump_config() {
  LOG_VALVE("", "Template Valve", this);
  ESP_LOGCONFIG(TAG,
                "  Has position: %s\n"
                "  Optimistic: %s",
                YESNO(this->has_position_), YESNO(this->optimistic_));
}

void TemplateValve::control(const ValveCall &call) {
#ifdef USE_TEMPLATE_VALVE_STOP_TRIGGER
  if (call.get_stop()) {
    this->stop_prev_trigger_();
    this->stop_trigger_->trigger();
    this->prev_command_trigger_ = this->stop_trigger_;
    this->publish_state();
  }
#endif
#ifdef USE_TEMPLATE_VALVE_TOGGLE_TRIGGER
  if (call.get_toggle().has_value()) {
    this->stop_prev_trigger_();
    this->toggle_trigger_->trigger();
    this->prev_command_trigger_ = this->toggle_trigger_;
    this->publish_state();
  }
#endif
  if (call.get_position().has_value()) {
    auto pos = *call.get_position();
    this->stop_prev_trigger_();

    if (pos == VALVE_OPEN) {
      this->open_trigger_->trigger();
      this->prev_command_trigger_ = this->open_trigger_;
    } else if (pos == VALVE_CLOSED) {
      this->close_trigger_->trigger();
      this->prev_command_trigger_ = this->close_trigger_;
    }
#ifdef USE_TEMPLATE_VALVE_POSITION_TRIGGER
    else {
      this->position_trigger_->trigger(pos);
    }
#endif

    if (this->optimistic_) {
      this->position = pos;
    }
  }

  this->publish_state();
}

ValveTraits TemplateValve::get_traits() {
  auto traits = ValveTraits();
  traits.set_is_assumed_state(this->assumed_state_);
#ifdef USE_TEMPLATE_VALVE_STOP_TRIGGER
  traits.set_supports_stop(this->has_stop_);
#else
  traits.set_supports_stop(false);
#endif
#ifdef USE_TEMPLATE_VALVE_TOGGLE_TRIGGER
  traits.set_supports_toggle(this->has_toggle_);
#else
  traits.set_supports_toggle(false);
#endif
#ifdef USE_TEMPLATE_VALVE_POSITION_TRIGGER
  traits.set_supports_position(this->has_position_);
#else
  traits.set_supports_position(false);
#endif
  return traits;
}

#ifdef USE_TEMPLATE_VALVE_POSITION_TRIGGER
Trigger<float> *TemplateValve::get_position_trigger() const { return this->position_trigger_; }
#endif

#ifdef USE_TEMPLATE_VALVE_STOP_TRIGGER
void TemplateValve::set_has_stop(bool has_stop) { this->has_stop_ = has_stop; }
#endif
#ifdef USE_TEMPLATE_VALVE_TOGGLE_TRIGGER
void TemplateValve::set_has_toggle(bool has_toggle) { this->has_toggle_ = has_toggle; }
#endif
#ifdef USE_TEMPLATE_VALVE_POSITION_TRIGGER
void TemplateValve::set_has_position(bool has_position) { this->has_position_ = has_position; }
#endif

void TemplateValve::stop_prev_trigger_() {
  if (this->prev_command_trigger_ != nullptr) {
    this->prev_command_trigger_->stop_action();
    this->prev_command_trigger_ = nullptr;
  }
}

}  // namespace esphome::template_
