import array as arr
import json
import csv
import mysql.connector
import os
import sys

verbosity_string = {0: '[NONE]', 1: '[ERROR]', 2: '[WARNING]', 3: '[INFO] ', 4: '[DEBUG]', 5: '[VERBOSE]'}
module_string = {0: 'cam_main', 1: 'esp_main', 2: 'cam_wifi', 3: 'cam_time', 4: 'esp_bmp', 5: 'esp_time', 6: 'esp_wifi'}

def send_data_to_db(parsed_log_file_name):
    print('send_data_to_db')
    # fetch data from csv file
    csv_path = parsed_log_file_name

    dict_list = list()
    with open(csv_path, "r") as csv_reader:
        csv_reader = csv.reader(csv_reader)
        for rows in csv_reader:
            dict_list.append({'log_time': rows[0], 'log_level': rows[1], 'module': rows[2], 'message': rows[3]})

    mydatabase = mysql.connector.connect(
        username='root',
        password='nebitno19',
        host='localhost',
        database='esp_log',
    )

    mycursor = mydatabase.cursor()

    for item in dict_list:
        sql = "INSERT INTO esp_log_t (log_time, log_level, module, message ) VALUES (%s ,%s, %s, %s)"
        val = item['log_time'], item['log_level'], item['module'], item['message']
        mycursor.execute(sql, tuple(val))
    mydatabase.commit()
    mycursor.execute('SELECT * FROM esp_log_t')
    myresult = mycursor.fetchall()
    for x in myresult:
        print(x)
    mydatabase.close()
    if os.path.exists(csv_path):
        os.remove(csv_path)


def parse_log(log_to_parse, parsed_log_file_name):
    one_log = arr.array('i')
    print(log_to_parse)
    for one_char in log_to_parse:
        one_log.append(one_char)
        if one_char == 59:
            time_info = ""
            for one_time in one_log[0:16]:
                time_info += str(chr(one_time))
            msg_id = one_log[16]
            log_level = str(verbosity_string.get(one_log[17] & 0x07))
            module_name = str(module_string.get((one_log[17] & 0xF8) >> 3))
            all_args = ["", "", ""]
            if one_log[18] != 59:
                del one_log[0:19]
                i = 0
                arg = ""
                for arg_char in one_log:
                    if arg_char != 58 and arg_char != 59:
                        arg += str(chr(arg_char))
                    else:
                        all_args[i] = arg
                        i = i + 1
                        arg = ""

            del one_log[:]
            msg_string = ""
            try:
                with open("./python_decoder/" + module_name + ".txt", "r") as fp:
                    # Load the dictionary from the file
                    id_to_str = json.load(fp)
                    msg_string = id_to_str.get(str(msg_id))
                    arg_nums = msg_string.count("?")
                    for i in range(0, arg_nums):
                        msg_string = msg_string.replace("?", all_args[i], 1)
                    fp.close()
            except FileNotFoundError as e:
                print(e)

            try:
                with open(parsed_log_file_name, "a") as fp:
                    fp.write(time_info + ',' + log_level + ',' + module_name + ',' + msg_string + '\n')
                    fp.close()
            except FileNotFoundError as e:
                print(e)
    send_data_to_db(parsed_log_file_name)

def read_log_file(file_name, parsed_log_file_name):
    print("read log file foo")
    try:
        with open(file_name, 'rb') as file:  # 'r', 'w', 'a'
            try:
                log_array = arr.array('i')
                while 1:
                    char = file.read(1)
                    if not char:
                        break
                    log_array.append(ord(char))
                file.close()
                if os.path.exists(file_name):
                    os.remove(file_name)
                parse_log(log_array, parsed_log_file_name)
            except TypeError as e:
                print(e)
    except FileNotFoundError as e:
        print(e)


if __name__ == '__main__':
    print('Hello from python')
    log_file_name = sys.argv[1]
    parsed_log_file_name = sys.argv[2]
    read_log_file(log_file_name, parsed_log_file_name)
