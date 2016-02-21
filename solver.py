#!/usr/bin/env python
# -*- utf-8 -*-

import re
import copy
import time

# This is const data
direct = ['u', 'r', 'd', 'l']
direct_vector = [{'x' : -1, 'y' : 0},
        {'x' : 0, 'y' : 1},
        {'x' : 1, 'y' : 0},
        {'x' : 0, 'y' : -1}]

def calculateNum(i, f, y):
    return i * y + f

def is_empty_not_change(map_data, i, f):
    if i >= map_data['x'] or i < 0:
        return False

    if f >= map_data['y'] or f < 0:
        return False
    return map_data['map'][i][f]

def is_empty(map_data, i, f):
    if i >= map_data['x'] or i < 0:
        return False

    if f >= map_data['y'] or f < 0:
        return False

    if not map_data['map'][i][f]:
        return False

    map_data['map'][i][f] = False
    return True

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
                this_row.append(False)
            else:
                this_row.append(True)
        map_data['map'].append(copy.deepcopy(this_row))

    '''
    for i in range(x):
        for f in range(y):
            if not map_data['map'][i][f]:
                print '#',
            else:
                print '*',
        print ''

    print 'this level x =', x, 'y =', y
    '''
    return map_data

def count_du_1(map_data):
    response = []
    for i in range(map_data['x']):
        for f in range(map_data['y']):
            if not map_data['map'][i][f]:
                continue

            du = 0
            for dir_enum in range(4):
                new_x = i + direct_vector[dir_enum]['x']
                new_y = f + direct_vector[dir_enum]['y']
                if is_empty_not_change(map_data, new_x, new_y):
                    du += 1
            if 1 == du:
                response.append([i, f])
    return response

def get_must_info(map_data, x, y):  # need update
    fill = 0
    flag_id = 0

    map_data_another = copy.deepcopy(map_data)

    def div_dfs(x, y):
        count = 0
        map_data_another['map'][x][y] = False
        for i in range(4):
            new_x = x + direct_vector[i]['x']
            new_y = y + direct_vector[i]['y']
            if is_empty(map_data_another, new_x ,new_y):
                count += div_dfs(new_x, new_y)

        return count + 1


    def dfs(x, y, flag):
        map_data['map'][x][y] = flag

        for i in range(4):
            new_x = x + direct_vector[i]['x']
            new_y = y + direct_vector[i]['y']
            if judge_empty(new_x, new_y):
                if isinstance(map_data['map'][new_x][new_y], bool):
                    dfs(new_x, new_y, flag)


    def judge_empty(x, y):
        if x < 0 or x >= map_data['x']:
            return False
        if y < 0 or y >= map_data['y']:
            return False
        if isinstance(map_data['map'][x][y], bool):
            return map_data['map'][x][y]
        return True

    for i in range(map_data['x']):
        for f in range(map_data['y']):
            if map_data['map'][i][f]:
                fill += 1
                du = 0
                first_x, first_y = i, f
                for dir_enum in range(4):
                    new_x = i + direct_vector[dir_enum]['x']
                    new_y = f + direct_vector[dir_enum]['y']
                    if True == judge_empty(new_x, new_y):
                        du += 1

                if du < 3:
                    map_data['map'][i][f] = flag_id
                    flag_id += 1

    if 0 == fill:
        return 0, True

    if div_dfs(first_x, first_y) != fill:
        return fill, False

    for i in range(map_data['x']):
        for f in range(map_data['y']):
            if (True == judge_empty(i, f)) and isinstance(map_data['map'][i][f], bool):
                dfs(i, f, flag_id)
                flag_id += 1

    #tongji

    edge_table = []
    for i in range(flag_id):
        edge_table.append([])

    du_count = [0] * flag_id
    for i in range(map_data['x']):
        for f in range(map_data['y']):
            if not isinstance(map_data['map'][i][f], bool):
                for dir_enum in [0, 3]:
                    new_x = i + direct_vector[dir_enum]['x']
                    new_y = f + direct_vector[dir_enum]['y']

                    if judge_empty(new_x, new_y):
                        point_foo = map_data['map'][i][f]
                        point_bar = map_data['map'][new_x][new_y]
                        if point_foo != point_bar:
                            du_count[point_foo] += 1
                            du_count[point_bar] += 1
                            edge_table[point_foo].append(point_bar)
                            edge_table[point_bar].append(point_foo)

    du_odd_count = 0
    du_zero_exist = False
    for point_enum in range(flag_id):
        if 0 == du_count[point_enum]:
            du_zero_exist = True
        if 1 == (du_count[point_enum] % 2):
            du_odd_count += 1

    # exist zero du 
    if len(du_count) > 1 and du_zero_exist:
        return fill ,False

    # all even point
    if 0 == du_odd_count:
        return fill, True
    if 2 != du_odd_count:
        return fill, False

    # start point enum, odd point equal 2
    for dir_enum in range(4):
        new_x = x + direct_vector[dir_enum]['x']
        new_y = y + direct_vector[dir_enum]['y']

        if judge_empty(new_x, new_y):
            start_id = map_data['map'][new_x][new_y]
            if 1 == (du_count[start_id] % 2):
                return fill, True
            for another_point in edge_table[start_id]:
                if 1 == (du_count[another_point] % 2):
                    return fill, True

    return fill, False

# just fit small 100 hash
def hash_function(map_data, x, y):
    response = 0
    for i in range(map_data['x']):
        for f in range(map_data['y']):
            response *= 2
            if map_data['map'][i][f]:
                response += 1
    response = response * 100 + x
    response = response * 100 + y
    # return hash(response)  conflict
    return response

def solve(map_data):
    hash_table = {}
    node = [0]

    def dfs(x, y, last_dir, path):
        node[0] += 1

        fill, is_continue = get_must_info(copy.deepcopy(map_use_dfs), x, y)

        if 0 == fill:
            return path

        if not is_continue:
            return 'no solution'

        hash_code = hash_function(map_use_dfs, x, y)
        if hash_code in hash_table:
            return 'no solution'
        hash_table[hash_code] = True

        if -1 == last_dir:
            dir_step = [0, 1, 2, 3]
        else:
            dir_step = [(last_dir + 5) % 4, (last_dir + 3) % 4]

        for dir_enum in dir_step:
            new_x, new_y = x + direct_vector[dir_enum]['x'], y + direct_vector[dir_enum]['y']
            step_forward = 0
            while is_empty(map_use_dfs, new_x, new_y):
                step_forward += 1

                new_x += direct_vector[dir_enum]['x']
                new_y += direct_vector[dir_enum]['y']

            new_x -= direct_vector[dir_enum]['x']
            new_y -= direct_vector[dir_enum]['y']

            # this direct is ok
            if step_forward >= 1:
                solution = dfs(new_x, new_y, dir_enum, path + direct[dir_enum])
                if 'no solution' != solution:
                    return solution

            step_x, step_y = x, y
            for step_enum in range(step_forward):
                step_x += direct_vector[dir_enum]['x']
                step_y += direct_vector[dir_enum]['y']
                map_use_dfs['map'][step_x][step_y] = True


        return 'no solution'


    response = {}

    # handle the du = 1
    du_1_vector = count_du_1(map_data)
    for du_1_enum in du_1_vector:
        x, y = du_1_enum[0], du_1_enum[1]
        print 'first from point', x, y, 'start dfs'
        map_use_dfs = copy.deepcopy(map_data)
        map_use_dfs['map'][x][y] = False
        solution = dfs(x, y, last_dir = -1, path = '')
        if 'no solution' != solution:
            response['x'] = x
            response['y'] = y
            response['path'] = solution
            print 'this level run', node[0], 'node'
            return response, node[0]

    # dfs from everygrid
    for i in range(map_data['x']):
        for f in range(map_data['y']):
            map_use_dfs = copy.deepcopy(map_data)

            if not is_empty(map_use_dfs, i, f):
                continue

            solution = dfs(x = i, y = f, last_dir = -1, path = '')
            if 'no solution' != solution:
                response['x'] = i
                response['y'] = f
                response['path'] = solution
                print 'this level run', node[0], 'node'
                return response, node[0]

    print 'this level run', node[0], 'node'
    return 'no solution', node[0]


# get map data from stdin
def main(map_info):
    map_data = parse_map(map_info)
    start_time = time.time()
    answer, node_num= solve(map_data)
    print answer
    print 'finish this level, cost', (time.time() - start_time), 's'
    print 'per second calculate', node_num / (time.time() - start_time), 'node'

    fd = open('answer', 'w')
    fd.write('x=' + str(answer['x']) + '&y=' + str(answer['y']) + '&path=' + str(answer['path']))
    fd.close()

    return answer

