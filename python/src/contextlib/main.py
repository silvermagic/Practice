#!/usr/bin/python
# -*- coding: utf-8 -*-
from contextlib import contextmanager

@contextmanager
def wapper(name):
    print("%s wake up" % name)
    yield
    print("%s sleep" % name)

if __name__ == '__main__':
    name = "fwd"
    with wapper(name):
        print("%s work all day" % name)
