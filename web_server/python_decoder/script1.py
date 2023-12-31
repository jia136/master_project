import array as arr
import json
import csv
import pathlib
import mysql.connector

verbosity_string = {0: '[NONE]', 1: '[ERROR]', 2: '[WARNING]', 3: '[INFO] ', 4: '[DEBUG]', 5: '[VERBOSE]'}
module_string = {0: 'esp_cam', 1: 'module_1', 2: 'module_2', 3: 'module_3'}


def send_data_to_db():
    print('send_data_to_db')
    # fetch data from csv file
    csv_path = "log_parsed.csv"

    dict_list = list()
    with open(csv_path, "r") as csv_reader:
        csv_reader = csv.reader(csv_reader)
        for rows in csv_reader:
            dict_list.append({'log_level': rows[0], 'module': rows[1], 'message': rows[2], 'arg0': rows[3], 'arg1': rows[4], 'arg2': rows[5]})

    mydatabase = mysql.connector.connect(
        username='root',
        password='nebitno19',
        host='localhost',
        database='esp_log',
    )

    mycursor = mydatabase.cursor()

    for item in dict_list:
        sql = "INSERT INTO esp_log_t ( log_level, module, message, arg0, arg1, arg2) VALUES (%s, %s, %s, %s, " \
                "%s, %s )"
        val = item['log_level'], item['module'], item['message'], item['arg0'], item['arg1'], item['arg2']
        mycursor.execute(sql, tuple(val))
    mydatabase.commit()
    mycursor.execute('SELECT * FROM esp_log_t')
    myresult = mycursor.fetchall()
    for x in myresult:
        print(x)
    mydatabase.close()


def parse_log(log_to_parse):
    one_log = arr.array('i')
    print(log_to_parse)
    for one_char in log_to_parse:
        one_log.append(one_char)
        if one_char == 59:
            msg_id = one_log[0]
            log_level = str(verbosity_string.get(one_log[1] & 0x07))
            module_name = str(module_string.get((one_log[1] & 0xF8) >> 3))
            all_args = ","
            no_of_arg = 0
            if one_log[2] != 59:
                del one_log[0:3]
                arg = ""
                for arg_char in one_log:
                    if arg_char != 58 and arg_char != 59:
                        arg += str(arg_char)
                    else:
                        all_args += arg
                        all_args += ","
                        arg = ""
                        no_of_arg = no_of_arg + 1

            # need to fill null data for MySQL table
            if no_of_arg == 0:
                all_args = ",-,-,-,"
            elif no_of_arg == 1:
                all_args += "-,-,"
            elif no_of_arg == 2:
                all_args += "-,"

            del one_log[:]
            msg_string = ""
            try:
                with open(module_name + ".txt", "r") as fp:
                    # Load the dictionary from the file
                    id_to_str = json.load(fp)
                    msg_string = id_to_str.get(str(msg_id))
                    fp.close()
            except FileNotFoundError as e:
                print(e)

            try:
                with open("log_parsed.csv", "a") as fp:
                    fp.write(log_level + ',' + module_name + ',' + msg_string + all_args[:-1] + '\n')
                    fp.close()
                    send_data_to_db()
            except FileNotFoundError as e:
                print(e)


def read_log_file(file_name):
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
                parse_log(log_array)
            except TypeError as e:
                print(e)
    except FileNotFoundError as e:
        print(e)


if __name__ == '__main__':
    print('Hello from python')
    log_file_name = "C:\\Users\\Jelena\\Desktop\\web_server\\python_decoder\\log_from_esp.txt"
    read_log_file(log_file_name)
