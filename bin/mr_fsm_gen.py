#!/usr/bin/env python

import yaml


def generate_header():
    pass


def generate_impl():
    passß


def main():
    with open('../examples/simple_fsm.yml', 'r') as stream:
        yml = yaml.load(stream)
    print yml

    generate_header()
    generate_impl()

if __name__ == '__main__':
    main()
