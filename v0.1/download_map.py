#!/usr/bin/env python
# -*- coding:utf-8 -*-

#import requesocks as requests
import requests
import solver
import re
import util

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


def get_data(level):
    url = data_url + '?level=' + str(level)
    raw_text = s.get(url).text
    raw_text = re.search('.*<br>', raw_text).group()[ : -4]

    util.file_write(level, raw_text)

def work():
    login()

    index = 0
    while True: 
        map_info = get_data(level = index)
        index += 1

if __name__ == '__main__':
    work()

