#!/usr/bin/python
# -*- coding: utf-8 -*-
from automaton import machines

def handler_on_enter(new_state, event):
    print("Entered '%s' due to '%s'" % (new_state, event))

def handler_on_exit(old_state, event):
    print("Exiting '%s' due to '%s'" % (old_state, event))

if __name__ == '__main__':
    # 创建有限状态机
    m = machines.FiniteMachine()

    # 添加状态到状态机(状态, 由其他状态转变成此状态时的处理, 由此状态转变成其他状态时的处理)
    m.add_state('red', on_enter=handler_on_enter, on_exit=handler_on_exit)
    m.add_state('green', on_enter=handler_on_enter, on_exit=handler_on_exit)
    m.add_state('yellow', on_enter=handler_on_enter, on_exit=handler_on_exit)

    # 添加状态转换(当前状态, 目标状态, 事件)
    m.add_transition('red', 'green', 'go')
    m.add_transition('green', 'yellow', 'slowdown')
    m.add_transition('yellow', 'red', 'stop')

    # 设置初始化状态
    m.default_start_state = 'red'

    # 初始化状态机
    m.initialize()
    print(m.pformat())

    # 运行状态机
    for event in ['go', 'slowdown', 'stop']:
        # 事件触发
        m.process_event(event)
        print("=============")
        print("Current state => %s" % m.current_state)
        print("=============")
        print(m.pformat())
