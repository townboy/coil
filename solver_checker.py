#! /usr/bin/env python
# -*- coding:utf-8 -*-

import sys
import copy
import util
import re

# return dict {
#   x = start x
#   y = start y
#   map = Boolen[][]
#         True represent already occupied
#         False represent empty
# }

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
            if '0' == str_map[util.calculateNum(i, f, y)]:
                this_row.append(False)
            else:
                this_row.append(True)
        map_data['map'].append(copy.deepcopy(this_row))

    for i in range(x):
        for f in range(y):
            if map_data['map'][i][f]:
                print '#',
            else:
                print '*',
        print ''

    print 'this level x =', x, 'y =', y
    return map_data

class Status():
    def __init__(self, map_data, now_x, now_y):
        self.map_info = copy.deepcopy(map_data)
        self.x = now_x
        self.y = now_y
        self.map_info['map'][now_x][now_y] = True

    # return True or False
    def is_finish(self):

        for i in range(self.map_info['x']):
            for f in range(self.map_info['y']):
                if not self.map_info['map'][i][f]:
                    return False
        return True

    # return True     occupied
    # return False    empty
    def is_point_occupy(self, x, y):
        if x >= self.map_info['x'] or x < 0:
            return True
        if y >= self.map_info['y'] or y < 0:
            return True

        return self.map_info['map'][x][y]


    # return True os False
    def goto(self, delta_x, delta_y):

        # count forward step
        # if count > 0 represent this direction is available
        forward_count = 0
        while not self.is_point_occupy(self.x + delta_x, self.y + delta_y):
            forward_count += 1
            self.x += delta_x
            self.y += delta_y
            self.map_info['map'][self.x][self.y] = True
        
        if forward_count > 0:
            return True
        return False
    
    def display(self):
        print 'in Status display function'
        for i in range(self.map_info['x']):
            for f in range(self.map_info['y']):
                if self.map_info['map'][i][f]:
                    print '#',
                else:
                    print '*',
            print ''


def run(map_info, answer):

    map_data = parse_map(map_info)

    map_instance = Status(map_data, answer['x'], answer['y'])
    
    print 'start point', answer['x'], answer['y']

    for one_dir in answer['path']:

        # get this step direction
        delta_x = util.direct_vector[ util.char2direct[one_dir] ]['x']
        delta_y = util.direct_vector[ util.char2direct[one_dir] ]['y']
        #map_instance.display()

        if False == map_instance.goto(delta_x, delta_y):
            return False
        if True == map_instance.is_finish():
            return True

    return False


if __name__ == '__main__':
    run()
