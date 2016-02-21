#! /usr/bin/env python
# -*- coding:utf-8 -*-

import re
import copy

# This is const data
direct2char = ['u', 'r', 'd', 'l']
direct_vector = [{'x' : -1, 'y' : 0},
        {'x' : 0, 'y' : 1},
        {'x' : 1, 'y' : 0},
        {'x' : 0, 'y' : -1}]

char2direct = {
        'u' : 0,
        'r' : 1,
        'd' : 2,
        'l' : 3
}


def calculateNum(i, f, y):
    return i * y + f

# return dict {
#   x = start x
#   y = start y
#   map = Boolen[][]
#         True represent already occupied
#         False represent empty
# }

def display_map(map_info):
    for i in range(map_info['x']):
        for f in range(map_info['y']):
            if map_data['map'][i][f]:
                print '#',
            else:
                print '*',
        print ''

def parse_map(raw_str):
    map_data = {}
    print 'start parse map info\n'
    print 'here is map info ' + raw_str

    # x is height , y is width
    x = int(re.search('x=[0-9]+&', raw_str).group()[2:-1])
    y = int(re.search('y=[0-9]+&', raw_str).group()[2:-1])
    str_map = raw_str[-x*y : ]

    map_data['x'] = x;
    map_data['y'] = y;

    map_data['map'] = []

    for i in range(x):
        this_row = []
        for f in range(y):
            if '1' == str_map[calculateNum(i, f, y)]:
                this_row.append(True)
            else:
                this_row.append(False)
        map_data['map'].append(copy.deepcopy(this_row))


    print 'this level x =', x, 'y =', y
    return map_data

def file_read(level):
    f = open('raw_map/' + str(level), 'r')
    map_info = f.read()
    f.close()
    return map_info


def file_write(level, map_info):
    f = open('raw_map/' + str(level), 'w')
    f.write(map_info)
    f.close()
    return map_info

