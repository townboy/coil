#! /usr/bin/env python
# -*- coding:utf-8 -*-

import solver
import sys
import solver_checker
import util
import re
import subprocess

def TurnMapInfo2XYMap(map_str):
    x = int(re.search('x=[0-9]+&', map_str).group()[2:-1])
    y = int(re.search('y=[0-9]+&', map_str).group()[2:-1])
    map_str = map_str[-x*y : ]

    return str(x), str(y), map_str

def MakeAnswer():
    answer = {}

    f = open('answer', 'r')

    x = f.readline()
    answer['x'] = int(x)

    y = f.readline()
    answer['y'] = int(y)

    path = f.readline()
    answer['path'] = path

    f.close()
    return answer
    

def run(level):
    print 'start run level', level
    map_info = util.file_read(level)
    x, y, map_str = TurnMapInfo2XYMap(map_info)

    subprocess.call('./src/coil ' + x + ' ' + y + ' ' +  map_str, shell = True)
    answer = MakeAnswer()

    #answer = solver.main(map_info)

    print '===================================================='
    if True == solver_checker.run(map_info, answer):
        print 'solver successful solve level ' ,level
        print '===================================================='
        return True
    else:
        print 'solver solve level', level, 'fail fail fail fail fail fail'
        print '===================================================='
        return False

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'must input level'
    elif 2 == len(sys.argv):
        run(sys.argv[1])
    else:
        for i in range(int(sys.argv[1]), int(sys.argv[2]) + 1 ):

            if False == run(str(i)):
                print '!!!!!!!!!!!!!!!!!!!!'
                print 'Solve level', i, 'FAIL FAIL FAIL FAIL'
                print '!!!!!!!!!!!!!!!!!!!!'
                break


