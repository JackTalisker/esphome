#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/template_lambda.h"
#include "esphome/components/switch/switch.h"

namespace esphome::template_ {

class TemplateSwitch final : public switch_::Switch, public Component {
 public:
  TemplateSwitch();

  void setup() override;
  void dump_config() override;

  template<typename F> void set_state_lambda(F &&f) { this->f_.set(std::forward<F>(f)); }
#ifdef USE_TEMPLATE_SWITCH_TURN_ON_TRIGGER
  Trigger<> *get_turn_on_trigger() const;
#endif
#ifdef USE_TEMPLATE_SWITCH_TURN_OFF_TRIGGER
  Trigger<> *get_turn_off_trigger() const;
#endif
  void set_optimistic(bool optimistic);
  void set_assumed_state(bool assumed_state);
  void loop() override;

  float get_setup_priority() const override;

 protected:
  bool assumed_state() override;

  void write_state(bool state) override;

  TemplateLambda<bool> f_;
  bool optimistic_{false};
  bool assumed_state_{false};
#ifdef USE_TEMPLATE_SWITCH_TURN_ON_TRIGGER
  Trigger<> *turn_on_trigger_;
#endif
#ifdef USE_TEMPLATE_SWITCH_TURN_OFF_TRIGGER
  Trigger<> *turn_off_trigger_;
#endif
#if defined(USE_TEMPLATE_SWITCH_TURN_ON_TRIGGER) || defined(USE_TEMPLATE_SWITCH_TURN_OFF_TRIGGER)
  Trigger<> *prev_trigger_{nullptr};
#endif
};

}  // namespace esphome::template_
