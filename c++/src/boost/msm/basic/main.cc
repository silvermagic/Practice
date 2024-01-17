//
// Created by 范炜东 on 2019/6/12.
//

#include <vector>
#include <iostream>
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>

using namespace boost::msm::front;

class Character {
public:
  virtual void on_idle() {
    std::cout << "Character::on_idle" << std::endl;
  }

  virtual void on_move() {
    std::cout << "Character::on_move" << std::endl;
  }

  virtual void on_attack() {
    std::cout << "Character::on_attack" << std::endl;
  }
};

// events
class idle {};
class move {};
class attack {};

class Idle : public boost::msm::front::state<> {
public:
  template <class Event,class FSM>
  void on_entry(Event const& e, FSM& fsm) {
    std::cout << "entering: Idle" << std::endl;
    fsm.character.on_idle();
  }

  template <class Event,class FSM>
  void on_exit(Event const&, FSM&) {
    std::cout << "leaving: Idle" << std::endl;
  }
};

class Move : public boost::msm::front::state<> {
public:
  template <class Event,class FSM>
  void on_entry(Event const& e, FSM& fsm) {
    std::cout << "entering: Move" << std::endl;
  }
};

class Attack : public boost::msm::front::state<> {
public:
  template <class Event,class FSM>
  void on_entry(Event const& e, FSM& fsm) {
    std::cout << "entering: Attack" << std::endl;
  }
};

class Idle2Move {
public:
  template <class Event, class FSM, class SourceState, class TargetState>
  void operator()(Event const&, FSM&, SourceState&, TargetState&)
  {
    std::cout << "Idle2Move" << std::endl;
  }
};

class Idle2Attack {
public:
  template <class Event, class FSM, class SourceState, class TargetState>
  void operator()(Event const&, FSM&, SourceState&, TargetState&)
  {
    std::cout << "Idle2Attack" << std::endl;
  }
};

class Status : public boost::msm::front::state_machine_def<Status> {
public:
  Status(Character& character) : character(character) {}

  template <class Event, class FSM>
  void on_entry(Event const&, FSM&)
  {
    std::cout << "entering: Status" << std::endl;
  }
  template <class Event, class FSM>
  void on_exit(Event const&, FSM&)
  {
    std::cout << "leaving: Status" << std::endl;
  }

  typedef Idle initial_state;

  class transition_table : public boost::mpl::vector<
          //    Start     Event         Next      Action                     Guard
          //  +---------+-------------+---------+---------------------------+----------------------+
          Row < Idle    , move        , Move    , Idle2Move                 , none                 >,
          Row < Idle    , attack      , Attack  , Idle2Attack               , none                 >,
          //  +---------+-------------+---------+---------------------------+----------------------+
          Row < Move    , idle        , Idle    , none                      , none                 >,
          Row < Move    , attack      , Attack  , none                      , none                 >,
          //  +---------+-------------+---------+---------------------------+----------------------+
          Row < Attack  , idle        , Idle    , none                      , none                 >,
          Row < Attack  , move        , Move    , none                      , none                 >
          //  +---------+-------------+---------+---------------------------+----------------------+
          > {};

  template <class Event, class FSM>
  void no_transition(Event const& e, FSM&, int state)
  {
    std::cout << "no transition from state " << state << " on event " << typeid(e).name() << std::endl;
  }

public:
  Character& character;
};

static char const* const state_names[] = { "Idle", "Move", "Attack" };
void state_show(boost::msm::back::state_machine<Status> const& s)
{
  std::cout << " -> " << state_names[s.current_state()[0]] << std::endl;
}

int main() {
  Character c;
  boost::msm::back::state_machine<Status> s(c);
  s.start();
  s.process_event(attack()); state_show(s);
  s.process_event(move()); state_show(s);
  s.process_event(idle()); state_show(s);
  s.stop();
}