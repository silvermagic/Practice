//
// Created by 范炜东 on 2019/6/13.
//

#include <memory>
#include <iostream>
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/internal_row.hpp>

using namespace boost::msm::front;

class Effect {
public:
  Effect(std::string s) : note(s) {}

  void show() {
    std::cout << note << std::endl;
  }

  std::string note;
};

class Character {
public:
  void show() {
    if (effect == nullptr) {
      std::cout << "nothing" << std::endl;
    } else {
      effect->show();
    }
  }
  std::shared_ptr<Effect> effect;
};

// events
class release {};
class timeout {};

class refresh
{
public:
  template <class Event,class FSM,class SourceState,class TargetState>
  void operator()(Event const&, FSM& fsm, SourceState&, TargetState&)
  {
    std::cout << "refresh: ";
    fsm.character->show();
  }
};

class Start : public boost::msm::front::state<> {
public:
  template <class Event,class FSM>
  void on_entry(Event const& e, FSM& fsm) {
    std::cout << "entering: Start" << std::endl;
    fsm.character->effect = fsm.effect;
  }

  class internal_transition_table : public boost::mpl::vector<
          //    Start     Event         Next      Action                     Guard
          //  +---------+-------------+---------+---------------------------+----------------------+
     Internal <           release               , refresh                   , none                 >
          //  +---------+-------------+---------+---------------------------+----------------------+
          > {};
};

class Stop : public boost::msm::front::state<> {
public:
  template <class Event,class FSM>
  void on_entry(Event const& e, FSM& fsm) {
    std::cout << "entering: Stop" << std::endl;
    fsm.character->effect = nullptr;
  }
};

class EffectStatus : public boost::msm::front::state_machine_def<EffectStatus> {
public:
  EffectStatus(std::shared_ptr<Character> c, std::shared_ptr<Effect> e) : character(c), effect(e) {}

  typedef Stop initial_state;

  class transition_table : public boost::mpl::vector<
          //    Start     Event         Next      Action                     Guard
          //  +---------+-------------+---------+---------------------------+----------------------+
          Row < Start   , timeout     , Stop    , none                      , none                 >,
          //  +---------+-------------+---------+---------------------------+----------------------+
          Row < Stop    , release     , Start   , none                      , none                 >
          //  +---------+-------------+---------+---------------------------+----------------------+
  > {};

  template <class Event, class FSM>
  void no_transition(Event const& e, FSM&, int state)
  {
    std::cout << "no transition from state " << state << " on event " << typeid(e).name() << std::endl;
  }

public:
  std::shared_ptr<Character> character;
  std::shared_ptr<Effect> effect;
};

static char const* const state_names[] = { "Start", "Stop" };
void state_show(boost::msm::back::state_machine<EffectStatus> const& s)
{
  std::cout << " -> " << state_names[s.current_state()[0]] << " inside: ";
  s.character->show();
}

int main() {
  std::shared_ptr<Character> c = std::make_shared<Character>();
  std::shared_ptr<Effect> e = std::make_shared<Effect>("freeze");
  boost::msm::back::state_machine<EffectStatus> s(c, e);
  // 可以不执行s.start()和s.stop()，但是这样就会跳过EffectStatus::on_entry和初始状态initial_state::on_entry，
  // 以及EffectStatus::on_exit和结束状态Stop::on_exit的调用
  s.process_event(release()); state_show(s);
  std::cout << "outside: ";c->show();
  s.process_event(release()); state_show(s);
  std::cout << "outside: ";c->show();
  s.process_event(timeout()); state_show(s);
  std::cout << "outside: ";c->show();
}
