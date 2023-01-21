#!/usr/bin/env python
# -*- coding:utf-8 -*-

#import requesocks as requests
import requests
import solver
import re
import subprocess

# write the map into raw_map/xxx

# This is const data 
login_url = 'http://www.qlcoder.com/auth/login'
data_url = 'http://www.qlcoder.com/train/autocr'
submit_url = 'http://www.qlcoder.com/train/crcheck?'

s = requests.session()

login_config = {
        'email' : '',
        'password' : '',
        'remember' : 'on',
        }

def login():
    cookie_response = s.get(login_url)
    cookie_str = re.search('_token" value=.*"', cookie_response.text).group()[15:-1]
    login_config['_token'] = cookie_str
    print 'cookie : ' + cookie_str
    print 'start login'
    login_response = s.post(login_url, data = login_config)
    print 'end login'
    print 'login status code : ' + str(login_response.status_code)

answer = {}
login()

def get_data():
    raw_text = s.get(data_url).text
    raw_text = re.search('.*<br>', raw_text).group()[ : -4]

    x = int(re.search('x=[0-9]+&', raw_text).group()[2:-1])
    y = int(re.search('y=[0-9]+&', raw_text).group()[2:-1])
    str_map = raw_text[-x*y : ]

    return str(x), str(y), str_map, raw_text

def send_answer(answer):
    submit_url_answer = submit_url + 'x=' + str(answer['x'] + 1) + '&y=' + str(answer['y'] + 1) + '&path=' + answer['path']
    submit_response = s.get(submit_url_answer)
    print submit_url_answer
    print submit_response.text

def file_write(map_str):
    raw_text = re.search('level=[0-9]*&', map_str).group()
    level = raw_text[6 : -1]

    f = open('raw_map/' + level, 'w')
    f.write(map_str)
    f.close()
    
while True:
    x, y, map_info, map_str= get_data()
    file_write(map_str)

    # comment out 
    # use solver.main like this
    #answer = solver.main(map_info)

    subprocess.call('./src/coil ' + x + ' ' + y + ' ' + map_info, shell = True)

    break

    print answer
    #send_answer(answer)
