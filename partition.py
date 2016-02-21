#! /usr/bin/env python
# -*- utf-8 -*-

'''
bug report
1. when dfs must contain the start point information
2. so we make one_solution must contain the start_point

'''

import re
import copy
import time
import Queue

START_INSIDE = 1
GET_IN = 2
GET_OUT = 4
FINISH = 999

direct_name = ['U', 'R', 'D', 'L']
direct_vector = [{'x' : -1, 'y' : 0},
        {'x' : 0, 'y' : 1},
        {'x' : 1, 'y' : 0},
        {'x' : 0, 'y' : -1}]

def construct_answer(x, y ,path):
    response = { 'x' : y,
            'y' : x,
            'path' : path
            }
    return response

def is_revert_direction(a, b):
    if ((a + b) % 2) == 0:
        return True
    return False

def this_position_is_empty(map_data, x, y):
    if x < 0 or x >= map_data['x']:
        return False
    if y < 0 or y >= map_data['y']:
        return False
    return map_data['map'][x][y]

'''
parse_map() 
return 
{
    'x' : x,
    'y' : y,
    'map' : [][] True or False
}
'''

def calculateNum(i, f, y):
    return i * y + f

def parse_map_from_html(html):

    # reverse x and y 
    # x is height , y is width
    map_data = {}
    Level = re.search('Level..[0-9]+', html).group()
    print 'now start', Level
    raw_str = re.search('FlashVars=.*"', html).group()[11:-1]
    y = int(re.search('x=[0-9]+&', raw_str).group()[2:-1])
    x = int(re.search('y=[0-9]+&', raw_str).group()[2:-1])
    str_map = raw_str[-x*y : ]

    map_data['x'] = x;
    map_data['y'] = y;

    map_data['map'] = []

    for i in range(x):
        this_row = []
        for f in range(y):
            if 'X' == str_map[calculateNum(i, f, y)]:
                this_row.append(False)
            else:
                this_row.append(True)
        map_data['map'].append(copy.deepcopy(this_row))

    for i in range(x):
        for f in range(y):
            if not map_data['map'][i][f]:
                print '#',
            else:
                print '*',
        print ''

    print 'this level x =', x, 'y =', y
    return map_data

def get_opposite_dir(x):
    return (x + 2) % 4


''' final status 
START_INSIDE after three numbers x, three numbers y
START_OUTSIDE no append data

GET_IN and GET_OUT after three numbers
(x, y, direction)
recode the in and the out direction

every path display the path from start_inside or get_in until get_out
'''

def make_start_point_limit_header(limit_id = -1):
    if -1 == limit_id:
        return 'no limit'
    return '%08d' % limit_id

def get_start_point_limit_header(status):
    if 'no limit' in status:
        return -1
    return int(status[:8])

def encode_start(is_start_inside, x = -1, y = -1):
    if is_start_inside:
        response = '%03d%03d%03d' % (START_INSIDE, x, y)
    else:
        response = ''

    return make_start_point_limit_header(-1) + response

def encode_body(is_get_in, x, y, direction):
    if is_get_in:
        response = '%03d%03d%03d%03d' % (GET_IN, x, y, direction)
    else:
        response = '%03d%03d%03d%03d' % (GET_OUT, x, y, direction)
    return response

def encode_ocupy():
    return '%03d' % FINISH

def judge_from_inside(encode_str):
    if int(encode_str[8:11]) == START_INSIDE:
        return True
    return False 

''' 
encode_status format

1. start_from_intern
2. start_from_extern
'''

''' 
encode_path format
path1:path2:path3
'''

tmp_count = [0]

# how to call
# find_solutin_for_one_block(flag_map, enum_block, flag_aim_num, flag_sum, block_point)
def find_solution_for_one_block(flag_map, handle_block, key_point, block_sum, block_point):

    # const define
    WALL = -1
    EMPTY = handle_block
    OCUPY = -2

    def print_entry_map():
        for i in range(map_x):
            for f in range(map_y):
                if None == entry_map[i][f]:
                    print '*',
                else:
                    print entry_map[i][f],
            print ''

    # entry_map record the get in direction
    def is_board_point(x, y):
        dir_list = []
        for enum_dir in range(4):
            next_x, next_y = x + direct_vector[enum_dir]['x'], y + direct_vector[enum_dir]['y']
            if next_x < 0 or next_x >= map_x:
                continue
            if next_y < 0 or next_y >= map_y:
                continue
            if -1 != flag_map[next_x][next_y] and handle_block != flag_map[next_x][next_y]:
                dir_list.append( (enum_dir + 2) % 4 )

        if 0 == len(dir_list):
            return False, -1
        else:
            return True, dir_list
    
    # return is_end, is_avail
    def is_end_status(dfs_map, bad_point):
        count = 0
        for point in block_point[handle_block - key_point]:
            x, y = point['x'], point['y']
            if EMPTY == dfs_map[x][y]:
                count += 1
        
        is_end = (0 == count)
        is_avail = (bad_point <= 1)
        
        return is_end, True
        #return is_end, is_avail

    def construct_path(path, is_new_path, direction):
        response = copy.deepcopy(path)
        if is_new_path:
            response.append('')
        response[-1] += direct_name[direction]
        return response


    def brute_force_dfs(x, y, encode_status, encode_path, dfs_map, bad_point, is_inside):
        tmp_count[0] += 1

        '''
        #define function
        print 'debug information'
        print_map(dfs_map, map_x, map_y)
        '''

        def this_position_is_ava(x, y):
            if x < 0 or x >= map_x:
                return False
            if y < 0 or y >= map_y:
                return False
            if WALL == dfs_use_map[x][y]:
                return False
            if EMPTY == dfs_use_map[x][y]:
                dfs_use_map[x][y] = OCUPY
                return True
            return False

        is_end, is_avail = is_end_status(dfs_map, bad_point)
        #end status, append this solution
        if is_end and is_avail:
            # push into solution
            new_encode_status = encode_status + encode_ocupy()
            solution[new_encode_status] = encode_path;
            #let is go out
            if 1 == len(block_point[handle_block - key_point] ) and judge_from_inside(encode_status) and is_inside:
                for enum_dir in range(4):
                    if None != entry_map[x][y] and enum_dir in entry_map[x][y]:
                        new_encode_status = encode_status + encode_body(False, x, y, enum_dir)
                        brute_force_dfs(x, y, new_encode_status, construct_path(encode_path, False, enum_dir), \
                                copy.deepcopy(dfs_map), bad_point, False)

        if not is_avail:
            return 

        # bug cannot go outside
        if True == is_inside:
            # goon next step
            for enum_dir in range(4):
                dfs_use_map = copy.deepcopy(dfs_map)
                x_plus, y_plus = direct_vector[enum_dir]['x'], direct_vector[enum_dir]['y']
                next_x, next_y = x + x_plus, y + y_plus
                step_forward = 0
                while this_position_is_ava(next_x, next_y):
                    step_forward += 1
                    next_x, next_y = next_x + x_plus, next_y + y_plus

                next_x, next_y = next_x - x_plus, next_y - y_plus

                # this dierct is ok
                if step_forward < 1:
                    continue

                # brute_force_dfs(next_x, next_y, encode_status, construct_path(encode_path, False, enum_dir), dfs_use_map, bad_point, True)

                # this point is way to go outside
                if None != entry_map[next_x][next_y]:
                    # can go outside stright, so go outside, or go inside with limits
                    if get_opposite_dir(enum_dir) in entry_map[next_x][next_y]:

                        # enum go outside
                        brute_force_dfs(-1, -1, encode_status + encode_body(False, next_x, next_y, enum_dir), \
                                construct_path(encode_path, False, enum_dir), dfs_use_map, bad_point, False)

                        # enum go inside with limit
                        out_x, out_y = next_x + x_plus, next_y + y_plus
                        next_block = flag_map[out_x][out_y]
                        limit_block = get_start_point_limit_header(encode_status)
                        if -1 == limit_block or limit_block == next_block:
                            new_encode_status = make_start_point_limit_header(next_block) + encode_status[8:]
                            brute_force_dfs(next_x, next_y, new_encode_status, construct_path(encode_path, False, enum_dir), \
                                    dfs_use_map, bad_point, True)

                    # enum the left and the right direction can go out
                    else:
                        # go inside without limit
                        brute_force_dfs(next_x, next_y, encode_status, construct_path(encode_path, False, enum_dir), dfs_use_map, bad_point, True)

                        # enum which direction go out, then go outside
                        for LR_dir in [(enum_dir + 3) % 4, (enum_dir + 1) % 4]:
                            if get_opposite_dir(LR_dir) in entry_map[next_x][next_y]:
                                new_path = construct_path( construct_path(encode_path, False, enum_dir) , False, LR_dir)
                                brute_force_dfs(-1, -1, encode_status + encode_body(False, next_x, next_y, LR_dir), \
                                       new_path, dfs_use_map, bad_point, False)
                            

                # this point can not go outside, so can go inside without limit
                else:
                    brute_force_dfs(next_x, next_y, encode_status, construct_path(encode_path, False, enum_dir), dfs_use_map, bad_point, True)


                '''
                # must go outside, go inside with limits
                if None != entry_map[next_x][next_y] and get_opposite_dir(enum_dir) in entry_map[next_x][next_y]:
                    brute_force_dfs(-1, -1, encode_status + encode_body(False, next_x, next_y, enum_dir), \
                            construct_path(encode_path, False, enum_dir), dfs_use_map, bad_point, False)

                    out_x, out_y = next_x + x_plus, next_y + y_plus
                    next_block = flag_map[out_x][out_y]
                    limit_block = get_start_point_limit_header(encode_status)
                    if -1 == limit_block or limit_block == next_block:
                        new_encode_status = make_start_point_limit_header(next_block) + encode_status[8:]
                        brute_force_dfs(next_x, next_y, new_encode_status, construct_path(encode_path, False, enum_dir), \
                                dfs_use_map, bad_point, True)
                    
                else:
                # go inside without limit
                    brute_force_dfs(next_x, next_y, encode_status, construct_path(encode_path, False, enum_dir), dfs_use_map, bad_point, True)
                '''


        else:
            # enum start from outside
            for point in block_point[handle_block - key_point]:
                x, y = point['x'], point['y']
                #get inside from this point
                if EMPTY == dfs_map[x][y] and entry_map[x][y] != None:
                    for enum_dir in entry_map[x][y]:
                        dfs_use_map = copy.deepcopy(dfs_map)
                        next_x, next_y = x, y
                        x_plus, y_plus = direct_vector[ enum_dir ]['x'], direct_vector[ enum_dir ]['y']

                        while this_position_is_ava(next_x, next_y):
                            next_x, next_y = next_x + x_plus, next_y + y_plus

                        next_x, next_y = next_x - x_plus, next_y - y_plus

                        #go inside
                        if entry_map[next_x][next_y] != None:
                            bad_point += 1

                        
                        '''
                        brute_force_dfs(next_x, next_y, encode_status + encode_body(True, x, y, enum_dir), \
                                construct_path(encode_path, True, enum_dir), dfs_use_map, bad_point, True)
                        '''

                        #can go outside
                        if None != entry_map[next_x][next_y] and get_opposite_dir(enum_dir) in entry_map[next_x][next_y]:
                            #go outside
                            brute_force_dfs(-1, -1, encode_status + encode_body(True, x, y, enum_dir) + encode_body(False, next_x, next_y, enum_dir), \
                                    construct_path(encode_path, True, enum_dir), dfs_use_map, bad_point, False)

                            #go inside must limit start block

                            out_x, out_y = next_x + x_plus, next_y + y_plus
                            next_block = flag_map[out_x][out_y]
                            limit_block = get_start_point_limit_header(encode_status)
                            if -1 == limit_block or limit_block == next_block:
                                new_encode_status = make_start_point_limit_header(next_block) + encode_status[8:] + encode_body(True, x, y, enum_dir)
                                brute_force_dfs(next_x, next_y, new_encode_status, construct_path(encode_path, True, enum_dir), dfs_use_map, bad_point, True)

                        #can go inside
                        else:
                            #go inside no limit
                            brute_force_dfs(next_x, next_y, encode_status + encode_body(True, x, y, enum_dir), \
                                    construct_path(encode_path, True, enum_dir), dfs_use_map, bad_point, True)
       

    map_x = len(flag_map)
    map_y = len(flag_map[0])
    entry_map = [[None] * map_y for enum in range(map_x)]

    for point in block_point[handle_block - key_point]:
        x, y = point['x'], point['y']
        is_board, entry_dir = is_board_point(x, y)
        if is_board:
            entry_map[x][y] = entry_dir
    
    print_entry_map()

    solution = {}

    # brute force find solutions
    #start from every intern grid

    for point in block_point[handle_block - key_point]:
        x, y = point['x'], point['y']
        if entry_map[x][y] == None:
            bad_point = 0
        else:
            bad_point = 1
        dfs_use_map = copy.deepcopy(flag_map)
        dfs_use_map[x][y] = OCUPY
        brute_force_dfs(x, y, encode_start(True, x, y), [''], dfs_use_map, bad_point, True)

    dfs_use_map = copy.deepcopy(flag_map)

    #start from outside
    brute_force_dfs(-1, -1, encode_start(False), [], dfs_map = dfs_use_map, bad_point = 0, is_inside = False)
        
    '''
    print 'debug solution in dfs search solution in one block'
    for item in solution:
        print item, solution[item]
    '''
        
    return solution


def this_position_is_aim_point(map_data, x, y):
    degree = 0
    if not this_position_is_empty(map_data, x, y):
        return False
    for enum_dir in range(4):
        point_x = x + direct_vector[enum_dir]['x']
        point_y = y + direct_vector[enum_dir]['y']
        
        if this_position_is_empty(map_data, point_x, point_y):
            degree += 1

    if 2 == degree:
        return True

def print_map(data, x, y):
    print 'just print map for display'
    for i in range(x):
        for f in range(y):
            print  '%3d' % data[i][f],
        print ''

def mark_in_aim_point(map_data):

    def this_position_is_not_color(x, y):
        if x < 0 or x >= map_data['x']:
            return False
        if y < 0 or y >= map_data['y']:
            return False
        if not map_data['map'][x][y]:
            return False
        if -1 == flag_map[x][y]:
            return True
        return False

    def bfs_color(x, y, color):
        bfs_queue = Queue.Queue()

        block_point[color - flag_aim_num].append( {'x' : x, 'y' : y} )
        bfs_queue.put( {'x' : x, 'y' : y} )
        flag_map[x][y] = color
        while not bfs_queue.empty():
            node = bfs_queue.get()
            node_x, node_y = node['x'], node['y']

            for enum_dir in range(4):
                next_x, next_y = node_x + direct_vector[enum_dir]['x'], node_y + direct_vector[enum_dir]['y']

                if this_position_is_not_color(next_x, next_y):
                    block_point[color - flag_aim_num].append( {'x' : next_x, 'y' : next_y} )
                    bfs_queue.put( {'x' : next_x, 'y' : next_y} )
                    flag_map[next_x][next_y] = color

    flag_map = [[-1] * map_data['y'] for row in range(map_data['x'])]
    flag_sum = 0
    flag_aim_num = 0
    block_point = []

    for i in range(map_data['x']):
        for f in range(map_data['y']):
            if this_position_is_aim_point(map_data, i, f):
                flag_map[i][f] = flag_sum
                flag_sum += 1

    flag_aim_num = flag_sum
    
    for i in range(map_data['x']):
        for f in range(map_data['y']):
            if map_data['map'][i][f] and this_position_is_not_color(i, f):
                block_point.append([])
                bfs_color(i, f, flag_sum)
                flag_sum += 1

    print 'debug'
    print flag_aim_num, flag_sum
    print_map(flag_map, map_data['x'], map_data['y'])

    single_solution = []

    for enum_block in range(flag_aim_num, flag_sum):
        single_solution.append( find_solution_for_one_block(flag_map, enum_block, flag_aim_num, flag_sum, block_point) )

    return flag_map, single_solution, flag_aim_num, flag_sum, block_point

            
def find_path(flag_map, single_solution, key_point, block_sum, block_point):

    def is_end(status):
        FLAG_FINISH = '%03d' % FINISH
        for block_status in status:
            if FLAG_FINISH not in block_status:
                return False
        return True

    def solution_is_avail(now_status, final_status):

        final_limit = get_start_point_limit_header(final_status)
        now_limit = get_start_point_limit_header(now_status)

        if -1 != final_limit and final_limit != now_limit:
            return False

        if final_status[8:11] != now_status[8:11]:
            return False
        str_START_INSIDE = '%03d' % START_INSIDE
        if now_status[8:11] == str_START_INSIDE:
            now_status = now_status[11:]
            final_status = final_status[17:]
        else:
            now_status = now_status[8:]
            final_status = final_status[8:]
            
        now_status_len = len(now_status)
        if now_status_len <= len(final_status) and now_status == final_status[:now_status_len]:
            return True
        return False

    # select solution we can use
    def select(block_in, block_status):
        avail_solution = {}

        for single_solution_path in single_solution[block_in - key_point]:
            if solution_is_avail(block_status, single_solution_path):
                avail_solution[single_solution_path] = single_solution[block_in - key_point][single_solution_path]

        return avail_solution

    #just handle two degrees points
    def this_point_is_empty_not_change(x, y, status):
        map_x = len(flag_map)
        map_y = len(flag_map[0])
        if x < 0 or x >= map_x:
            return False
        if y < 0 or y >= map_y:
            return False

        handle_block = flag_map[x][y]
        if -1 == handle_block:
            return False

        FLAG_FINISH = '%03d' % FINISH
        if FLAG_FINISH in status[handle_block]:
            return False
        return True

    def find_position(block_in):
        map_x = len(flag_map)
        map_y = len(flag_map[0])
        for i in range(map_x):
            for f in range(map_y):
                if flag_map[i][f] == block_in:
                    return i, f
        return -1, -1

    def delete_pre(now_status, next_status):
        FLAG_START = '%03d' % START_INSIDE
        response = next_status
        if FLAG_START in now_status[8:11]:
            now_status = now_status[11:]
            next_status = next_status[17:]
        else:
            now_status = now_status[8:]
            next_status = next_status[8:]

        return next_status[len(now_status):]

    def seek_path(block, now_status):

        FLAG_START = '%03d' % START_INSIDE
        
        length = len(now_status) - 8
        if FLAG_START in now_status[8:11]:
            length += 9

        nth = length / 24

        return 'FFF%06d%03d' % (block, nth)

    def seek_available_solution(dfs_status, final_status):

        FLAG_START = '%03d' % START_INSIDE

        for one_status in final_status:
            
            final_limit = get_start_point_limit_header(one_status)
            dfs_limit = get_start_point_limit_header(dfs_status)

            if -1 != final_limit and final_limit != dfs_limit:
                continue

            # this is start point
            if FLAG_START in dfs_status[8:11]:
                delete_one_status = one_status[8:11] + one_status[17:]
                if delete_one_status == dfs_status[8:]:
                    return final_status[one_status]
            # this is not start point
            else:
                if dfs_status[8:] == one_status[8:]:
                    return final_status[one_status]

        return 'seek_available_solution_error'

    def seek_available_solution_for_start_point(dfs_status, final_status):

        start_x, start_y = -1, -1
        for one_status in final_status:
            
            final_limit = get_start_point_limit_header(one_status)
            dfs_limit = get_start_point_limit_header(dfs_status)

            if -1 != final_limit and final_limit != dfs_limit:
                continue

            delete_one_status = one_status[8:11] + one_status[17:]
            if delete_one_status == dfs_status[8:]:
                start_x, start_y = int(one_status[11:14]), int(one_status[14:17])
                break

        return start_x, start_y

    def make_final_path(status, path, start_block):

        FLAG_START = '%03d' % START_INSIDE
        FLAG_FINISH = '%03d' % FINISH
        map_x = len(flag_map)
        map_y = len(flag_map[0])

        response_path = ''

        # get solution_select
        solution_select = []
        for count in range(block_sum - key_point):
            solution_select.append(seek_available_solution(status[count + key_point], single_solution[count]) )

        # get start_x and start_y
        if start_block < key_point:
            for i in range(map_x):
                for f in range(map_y):
                    if start_block == flag_map[i][f]:
                        start_x, start_y = i, f
        else:
            start_x, start_y = seek_available_solution_for_start_point(status[start_block], single_solution[start_block - key_point])

        #bug
        # bug select one solution for one block
        for single_path in path:
            if 'FFF' in single_path:
                block, nth = int(single_path[3:9]), int(single_path[9:12])
                response_path += solution_select[block - key_point][nth]

            else:
                response_path += single_path

        correct_path = response_path[0]
        path_length = len(response_path)
        for i in range(1, path_length):
            if response_path[i] != response_path[i-1]:
                correct_path += response_path[i]
                
        return start_x, start_y, correct_path

    def make_answer(status, path):
        response = {}
        FLAG_START = '%03d' % START_INSIDE
        FLAG_FINISH = '%03d' % FINISH
        for enum_block in range(block_sum):
            if FLAG_START in status[enum_block][8:11]:
                start_block = enum_block

        response['y'], response['x'], response['path'] = make_final_path(status, path, start_block)
        return response 


    def dfs_path(block_in, status, path):

        FLAG_FINISH = '%03d' % FINISH
        
        if is_end(status):
            return make_answer(status, path)

        # go on
        # two degrees point
        if block_in < key_point:
            x, y = find_position(block_in)
            for enum_dir in range(4):
                next_x, next_y = x + direct_vector[enum_dir]['x'], y + direct_vector[enum_dir]['y']
                if not this_point_is_empty_not_change(next_x, next_y, status):
                    continue

                next_block = flag_map[next_x][next_y]
                #goto a key point and check available
                if next_block < key_point:
                    if FLAG_FINISH in status[next_block]:
                        continue

                    new_status = copy.deepcopy(status)
                    new_status[next_block] += FLAG_FINISH

                    new_path = copy.deepcopy(path)
                    new_path.append( direct_name[enum_dir] )
                    answer = dfs_path(next_block, new_status, new_path)
                    if answer != 'no solution':
                        return answer

                #goto a non key point
                else:
                    # check available
                    append_status = '%03d%03d%03d%03d' % (GET_IN, next_x, next_y, enum_dir)
                    if append_status in status[next_block]:
                        continue
                    
                    new_status = copy.deepcopy(status)
                    new_status[next_block] += append_status

                    new_path = copy.deepcopy(path)
                    new_path.append( direct_name[enum_dir] )
                    answer = dfs_path(next_block, new_status, new_path)
                    if answer != 'no solution':
                        return answer
        else:
            avail_select = select(block_in, status[block_in])
            #seek next step
            for one_select in avail_select:
                str_status = delete_pre(status[block_in], one_select)
                if 0 == len(str_status):
                    continue

                new_status = copy.deepcopy(status)
                new_path = copy.deepcopy(path)

                # finish now
                if FLAG_FINISH in str_status[:3]:
                    # append FLAG_FINISH
                    new_status[block_in] += str_status[:3]
                    new_path.append(seek_path(block_in, status[block_in]))

                    answer = dfs_path(block_in, new_status, new_path)
                    if answer != 'no solution':
                        return answer
                    continue

                # two case must go out
                now_x, now_y, out_dir = int(str_status[3:6]), int(str_status[6:9]), int(str_status[9:12])
                next_x, next_y = now_x + direct_vector[out_dir]['x'], now_y + direct_vector[out_dir]['y']
                next_block = flag_map[next_x][next_y]
                
                # this key point already be ocupy
                if FLAG_FINISH in status[next_block]:
                    continue

                new_status[next_block] += FLAG_FINISH
                new_path.append(seek_path(block_in, status[block_in]))
                new_path.append( direct_name[out_dir] )
                
                # go out and this block has not finished
                if FLAG_FINISH not in str_status[12:15]:

                    new_status[block_in] += str_status[:12]
                    answer = dfs_path(next_block, new_status, new_path)
                    if answer != 'no solution':
                        return answer
                    continue

                # go out and this block has finished  append FLAG_FINISH
                new_status[block_in] += str_status[:15]
                answer = dfs_path(next_block, new_status, new_path)
                if answer != 'no solution':
                    return answer

        return 'no solution'


    status = [ '' for count in range(block_sum) ]

    for enum_block in range(block_sum):
        use_status = copy.deepcopy(status)
        if enum_block < key_point:
            for count in range(block_sum):
                use_status[count] = '%08d' % enum_block
        else:
            for count in range(block_sum):
                use_status[count] = 'no limit'

        use_status[enum_block] += '%03d' % START_INSIDE
        if enum_block < key_point:
            use_status[enum_block] += '%03d' % FINISH
        answer = dfs_path(enum_block, use_status, [])

        if 'no solution' != answer:
            return answer

    return 'no solution'

def initialize(map_data):
    return mark_in_aim_point(map_data)

def run(html):
    old_count = tmp_count[0]
    print direct_name
    map_data_from_html = parse_map_from_html(html)
    flag_map, single_solution, key_point, block_sum, block_point = initialize(map_data_from_html)
    print 'this level run', old_count - tmp_count[0] , 'nodes'
    return find_path(flag_map, single_solution, key_point, block_sum, block_point)
