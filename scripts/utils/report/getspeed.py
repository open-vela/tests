#!/usr/bin/python3
# encoding: utf-8
import json
import os
import re
from HTMLTable import HTMLTable
import csv
import glob
from collections import defaultdict

import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
sys.path.append('../../')
from utils.common import getConfigValue, findFile

def getFilePath(log_path, name):
    '''
    input :
    log_path :log file path
    name : target log part file name
    return:
    files : files name
    '''
    file_name = []
    for root, dirs, files in os.walk(log_path):
        for file in files:
            if name in file:
                file_name.append(os.path.join(root, file))
    return file_name

def getAllLog(file_name):
    '''
    input :
    file_name :log file name
    return :
    data : data in a list
    '''
    file = open(file_name, "r", encoding="utf-8", errors="ignore")
    file_data = file.readlines()
    file.close()
    return file_data

def drawHtml(p, content, type="ramspeed", add=None, not_found=False):
    '''
    json_path: a json file keep the ap/audio base speed
    p :
    content : speed data from log
    html_id : creat the html's name
    type : ramspeed / cachespeed
    '''
    dict_memcpy = getDict(content, type, add, not_found)
    dict_memset = None
    if "ramspeed" in type:
        dict_memset = getDict(content, type, "memset", not_found)
    creatEmailCsv(p, dict_memcpy, type, add, dict_memset)

def creatEmailCsv(p, dict_memcpy, type, add=None, dict_memset=None, compare=None):
    '''
    input:
    json_path : json_path: a json file keep the ap/audio base speed
    board : p.board
    core : p.core
    table_name : html table name
    dict_memcpy : get the speed from log
    type : ramspeed or cachespeed
    return:
    html
    '''
    if p.ci:
        return
    table = []
    table_hander = ()
    save_name = ""
    if "ramspeed" in type:
        table_hander = ('Board', 'Core', 'Name', 'Size', 'System memcpy(KB/s)', 'System memset(KB/s)')
    elif type == "cachespeed" and add == "avg":
        table_hander = ('Board', 'Core', 'Name', 'Size', 'Invalidate(ns)', 'Clean(ns)', 'Flush(ns)', 'Icache(ns)')
    elif type == "cachespeed" and add == "rate":
        table_hander = ('Board', 'Core', 'Name', 'Size', 'Invalidate(KB/s)', 'Clean(KB/s)', 'Flush(KB/s)', 'Icache(KB/s)')
    elif "free" in type:
        table_hander = ('Board', 'Core', 'Name', 'Total','Used','Free', 'Largest')
    elif type == "df-h":
        table_hander = ('Board', 'Core', 'Name', 'FileSystem', 'Mounted on', 'Size', 'Available')
    elif type == "irqinfo":
        table_hander = ('Board', 'Core', 'Name', 'IRQ', 'time(us)')
    elif type == "critmon":
        table_hander = ('Board', 'Core', 'Name', 'Pid', 'Description', 'Pre-empion(s)','Csection(s)')
    elif type in ["coremark", "getprime"]:
        table_hander = ('Board', 'Core', type)
    elif type == "fs":
        table_hander = ('Board', 'Core', 'Name', 'Size', 'Write(KB/s)', 'Read(KB/s)')
    table.append(table_hander)

    for key in dict_memcpy:
        j = 0
        if "free" in type:
            a = (p.board, p.core, type, )
        elif type in ["coremark", "getprime"]:
            a = (p.board, p.core, )
            if p.core == None:
                a = (p.board, "None", )
        else:
            a = (p.board, p.core, type, key,)
        f3 = ()
        if type in ["coremark", "getprime"]:
            f3 = (dict_memcpy[type],)
        elif "ramspeed" in type:
            f3 += (dict_memcpy[key][j], dict_memset[key][j])
        else:
            while j < len(dict_memcpy[key]):
                f3 += (dict_memcpy[key][j],)
                j += 1

        a += f3
        table.append(a)

    path_parts = p.log_path.split(os.sep)
    # print(f'path_parts = {path_parts}')
    for i in range(len(path_parts)):
        folder_name = path_parts[i]
        # find 20230616xxxx
        if len(folder_name) == 12 and folder_name.isdigit():
            # where is performance
            per_folder_path = os.path.join('/', *path_parts[:i+1], 'performance')
            if not os.path.exists(per_folder_path):
                os.makedirs(per_folder_path)
            # print(f'os.path.exists(per_folder_path) = {os.path.exists(per_folder_path)}')
            if p.core:
                save_name = p.board + "_" + p.core + "_" + type
            else:
                save_name = p.board + "_" + type
            if type == "cachespeed":
                save_name = save_name + '_' + add + '.csv'
            else:
                save_name = save_name + '.csv'

            save_name = os.path.join(per_folder_path, save_name)
    # print(f'save = {save_name}')
    if (save_name != ""):
        f = open(save_name, 'w', encoding='utf8', errors="ignore", newline='')
        writer = csv.writer(f)
        for line in table:
            writer.writerow(line)
        f.close()


def get_date(base_path):
    date = None
    path_list = base_path.split('/')
    for d in path_list:
        if d.isdigit():
            date = d
            break
    return date

def getHtml(type, log_path):
    '''
    input :
    file_name : html name
    file_content: html content
    return :
    None
    '''
    # search in /buildarea/xxxxx/xxxx/202302230635 find type_basedata.csv
    csv_path = log_path
    a = findFile(csv_path, type, '.csv', None, False)
    for x in a:
        a = [x for x in a if '.csv' in x]
    if a == []:
        return None

    today_date = get_date(log_path)

    # merge all csv
    table_list = []
    for path in a:
        f = open(path, "r")
        flag = False
        for line in f:
            line = line.replace("\n", "")
            ls = line.split(",")
            ls = tuple(ls)
            if flag:
                table_list.append(ls)
            flag = True
    table = HTMLTable(caption=type)
    add = None
    if "cachespeed" in type:
        all_type = type.split('_')
        if len(all_type) >= 2:
            add = all_type[1]
            type = "cachespeed"
        else:
            return None

    if "ramspeed" in type:
        table.append_header_rows((
            ('Board', 'Core', 'Name', 'Size', today_date, ''),
            (' ',     ' ',    ' ',    ' ',    'System memcpy(KB/s)', 'System memset(KB/s)')
        ))
        table[0][0].attr.rowspan = 2
        table[0][1].attr.rowspan = 2
        table[0][2].attr.rowspan = 2
        table[0][3].attr.rowspan = 2
        table[0][4].attr.colspan = 2
    elif type == "cachespeed" and add == "avg":
        table.append_header_rows((
            ('Board', 'Core', 'Name', 'Size', today_date, '', '', ''),
            (' ',     ' ',    ' ',    ' ', 'Invalidate(ns)', 'Clean(ns)', 'Flush(ns)', 'Icache(ns)')
        ))
        table[0][0].attr.rowspan = 2
        table[0][1].attr.rowspan = 2
        table[0][2].attr.rowspan = 2
        table[0][3].attr.rowspan = 2
        table[0][4].attr.colspan = 4
    elif type == "cachespeed" and add == "rate":
        table.append_header_rows((
            ('Board', 'Core', 'Name', 'Size', today_date, '', '', ''),
            (' ',     ' ',    ' ',    ' ', 'Invalidate(KB/s)', 'Clean(KB/s)', 'Flush(KB/s)', 'Icache(KB/s)')
        ))
        table[0][0].attr.rowspan = 2
        table[0][1].attr.rowspan = 2
        table[0][2].attr.rowspan = 2
        table[0][3].attr.rowspan = 2
        table[0][4].attr.colspan = 4

    elif type == "free":
        table.append_header_rows((
            ('Board', 'Core', 'Name', today_date, '',     ''),
            (' ',     ' ',    ' ',    'Total',    'Free', 'Largest')
        ))
        table[0][0].attr.rowspan = 2
        table[0][1].attr.rowspan = 2
        table[0][2].attr.rowspan = 2
        table[0][3].attr.colspan = 3
    elif type == "df-h":
        table.append_header_rows((
            ('Board', 'Core', 'Name', 'FileSystem', 'Mounted on', today_date, ''),
            (' ',     ' ',    ' ',    ' ',          ' ',          'Size',     'Available')
        ))
        table[0][0].attr.rowspan = 2
        table[0][1].attr.rowspan = 2
        table[0][2].attr.rowspan = 2
        table[0][3].attr.rowspan = 2
        table[0][4].attr.rowspan = 2
        table[0][5].attr.colspan = 2
    elif type == "irqinfo":
        table.append_header_rows((
            ('Board', 'Core', 'Name', 'IRQ', today_date),
            (' ',     ' ',    ' ',    ' ',   'time(us)')
        ))
        table[0][0].attr.rowspan = 2
        table[0][1].attr.rowspan = 2
        table[0][2].attr.rowspan = 2
        table[0][3].attr.rowspan = 2
    elif type == "critmon":
        table.append_header_rows((
            ('Board', 'Core', 'Name', 'Pid', 'Description', today_date,    ''),
            (' ',     ' ',    ' ',    ' ',   ' ',          'Pre-empion(s)','Csection(s)')
        ))
        table[0][0].attr.rowspan = 2
        table[0][1].attr.rowspan = 2
        table[0][2].attr.rowspan = 2
        table[0][3].attr.rowspan = 2
        table[0][4].attr.rowspan = 2
        table[0][5].attr.colspan = 2
    elif type in ["coremark", "getprime"]:
        table.append_header_rows((
            ('Board', 'Core', type),
        ))
    elif type == "fs":
        table.append_header_rows((
            ('Board', 'Core', 'Name', 'Size', today_date, ''),
            (' ', ' ', ' ', ' ', 'Write(KB/s)', 'Read(KB/s)',)
        ))
        table[0][0].attr.rowspan = 2
        table[0][1].attr.rowspan = 2
        table[0][2].attr.rowspan = 2
        table[0][3].attr.rowspan = 2
        table[0][4].attr.colspan = 2

    for x in table_list:
        table.append_data_rows((x,))

    table.caption.set_style({
        'font-size': '15px',
    })
    table.set_style({
        'border-collapse': 'collapse',
        'word-break': 'keep-all',
        'white-space': 'nowrap',
        'font-size': '14px',
    })
    table.set_cell_style({
        'border-color': '#000',
        'border-width': '1px',
        'border-style': 'solid',
        'padding': '5px',
    })
    table.set_header_row_style({
        'color': '#fff',
        'background-color': '#48a6fb',
        'font-size': '18px',
    })
    table.set_header_cell_style({
        'padding': '15px',
    })
    table[1].set_cell_style({
        'padding': '8px',
        'font-size': '15px',
    })

    html = table.to_html()
    return html


def saveHtml(file_name, file_content):
    with open(file_name, "wb") as f:
        f.write(file_content.encode())

def find(content, str1, str2):
    return re.findall(str1 +'(.*?)'+ str2, content)


def getCache(contend, type="ramspeed", add=None):
    name = ["32 B", "64 B", "128 B", "256 B", "512 B", "1 KB", "2 KB", "4 KB", "8 KB", "16 KB"]
    dict = defaultdict(lambda: ['FAILED', 'FAILED', 'FAILED', 'FAILED'])

    i = 0
    need = ["dcache invalidate", "dcache clean", "dcache flush", "icache invalidate"]
    f = 0
    while i < len(contend):
        if "unalign" in contend[i]:
            f = 0
        elif "align" in contend[i]:
            genre = find(contend[i], r"\*\* ", r" \[rate, avg, cost\]")
            if genre:
                f = 1
                indexs = need.index(genre[0])
        elif f:
            pattern = r'(\d+)\sBytes:\s([\d\.]+),\s(\d+),\s(\d+)'
            match = re.findall(pattern, contend[i])
            if match:
                block = match[0][0]
                values = match[0][1:]
                if len(block) >= 4:
                    block = str(int(block) // 1024) + " KB"
                else:
                    block = block + " B"
                if add == "avg" and len(values) >= 2:
                    dict[block][indexs] = values[1]
                elif add == "rate" and len(values) >= 1:
                    dict[block][indexs] = values[0]

        i += 1
    return dict

def getDict(contend, type="ramspeed", add=None, not_found=False):
    '''
    input:
    contend : the all lines from log
    return:
    dict_memcpy : dict_memcpy[Size] = speed
    '''
    dict = {}
    name = ['512 B', '1 KB', '2 KB', '4 KB', '8 KB', '16 KB', '32 KB', '64 KB',
            '128 KB', '256 KB', '512 KB']
    if type == "ramspeed_nor_flash":
        name = ['512 B', '1 KB', '2 KB', '4 KB', '8 KB', '16 KB', '32 KB', '64 KB',
            '128 KB']

    # init the dict
    if "ramspeed" in type:
        for key in name:
            dict[key] = ['FAILED']
            if not_found:
                dict[key] = ['NOT FOUND']
    else:
        for key in name:
            dict[key] = ['FAILED', 'FAILED', 'FAILED', 'FAILED']
            if not_found:
                dict[key] = ['NOT FOUND', 'NOT FOUND', 'NOT FOUND', 'NOT FOUND']

    more = ""
    if "ramspeed" in type:
        more = add
    start = 0
    str1 = 'do '
    str2 = ' operation'
    if "ramspeed" in type:
        str1 = 'Perform '
        str2 = ' access'
    while start < len(contend):
        # 1. find ______xxx performance______
        while start < len(contend) and find(contend[start], more, 'performance') == []:
            start += 1
        if start == len(contend):
            break
        genre = find(contend[start], more,  'performance')[0]
        end = start + 1
        while end < len(contend) and find(contend[end], more, 'performance') == []:
            end += 1
        # 2. get ______do xxx operation______
        i = start
        while i < end:
            while i < end and find(contend[i], str1, str2) == []:
                i += 1
            if i == end:
                break
            size = find(contend[i], str1, str2)[0]
            if "ramspeed" in type:
                size = size[0: -4]
            j = i + 1
            while j < end and find(contend[j], str1, str2) == []:
                j += 1
            speed = []
            if size in name:
                for k in range(i, j):
                    avg = []
                    if type == "cachespeed":
                        if add == "avg":
                            avg = find(contend[k], 'avg = ', ' ns')
                        else:
                            avg = find(contend[k], 'Rate: ', ' KB/s')
                    else:
                        if find(contend[k], 'system ' + add + '()', ''):
                            avg = find(contend[k], 'Rate = ', ' KB/s')
                    if len(avg):
                        speed.append(int(float(avg[0])))
            if len(speed):
                if "ramspeed" in type:
                    dict[size][0] = speed[0]
                else:
                    if genre == "dcache":
                        while len(speed) < 3:
                            speed.append("FAILED")
                        dict[size][0] = speed[0]
                        dict[size][1] = speed[1]
                        dict[size][2] = speed[2]
                    else:
                        while len(speed) < 1:
                            speed.append("FAILED")
                        dict[size][3] = speed[0]
            i = j
        start += 1
    return dict

def getContent(p, data, type="ramspeed", add=None, not_found=True):
    '''
    json_path: a json file keep the ap/audio base speed
    data : log data
    type: ramspeed or cachespeed
    '''
    dict_len = {}
    dict_len = defaultdict(lambda : 300)
    dict_len["free"] = 3
    dict_len["df -h"] = 10
    flag = 0
    content = []
    maxlen = dict_len[type]
    target = getConfigValue(
        p.path, p.board, core=p.core, flag="NSH_PROMPT_STRING"
    )
    if target[-1] != '>':
        target = target + '>'
    type1 = type
    if "ramspeed" in type:
        type1 = type
        type = "ramspeed"
    for lines in data:
        # the max size
        if len(content) >= maxlen:
            break
        if flag == 1:
            if target in lines:
                break
            else:
                content.append(lines.strip())
        if target in lines:
             if type in lines:
                flag = 1

    type = type1
    if "ramspeed" in type or type == "cachespeed":
        if len(content):
            drawHtml(p, content, type, add, not_found)
    else:
        return content
