# author: github.com/run4w4y, add4che@gmail.com
# if you have found any bugs please let me know

import os


def error(error_msg):
    if config.get('error_displaying') == '1' or config.get('error_displaying') is None:
        print('error: '+error_msg)
        input('press any key')
    exit(0)


def check_start():
    if output_line.count('{') > 1:
        error("expected only one '{' in the end of the line " + str(i))
    if tmp[-2] != '{':
        error("expected '{' in the end of the line " + str(i))


def change_defined_funcs(s):
    s = s.replace('+', ' + ')
    s = s.replace('-', ' - ')
    s = s.replace('*', ' * ')
    s = s.replace('/', ' / ')
    s = s.replace('=', ' = ')
    s = s.replace('(', ' ( ')
    s = s.replace(')', ' ) ')

    s_split = s.split()
    s = ''

    for it in local_vars:
        if s_split.count(it) != 0:
            for jt in range(len(s_split)):
                if s_split[jt] == it:
                    s_split[jt] = 'f.get("'+it+'")'

    for it in s_split:
        s += it + ' '

    while s.count('( ') != 0:
        s = s.replace('( ', '(')

    s_split = s.split()
    s = ''

    for it in range(len(s_split)):
        if funcs.get(s_split[it]) is not None:
            s_split[it] = funcs.get(s_split[it])
        if func_props.get(s_split[it]) is not None:
            args_num, tmp_index = func_props.get(s_split[it])
            args_cur = ''
            if it+1 != range(len(s_split))[-1]+1 and s_split[it+1][0] == '(':
                args_cur = s_split[it+1]
            else:
                error("couldn't find call parameters in the line "+str(i))

            if args_cur.count(',')+1 != args_num and len(args_cur) != 2:
                error("found "+str(args_cur.count(',')+1)+" parameters instead of "+str(args_num)+" in the line "+str(i))

            s_split[it+1] = s_split[it+1][1:]
            if args_cur != '()':
                s_split[it] = 'f.call' + str(args_num) + '("' + s_split[it] + '",'
            else:
                s_split[it] = 'f.call0("' + s_split[it] + '"'

        s += s_split[it] + ' '

    s = s.replace('+', ' + ')
    s = s.replace('-', ' - ')
    s = s.replace('*', ' * ')
    s = s.replace('/', ' / ')
    s = s.replace('=', ' = ')
    s = s.replace('(', ' ( ')
    s = s.replace(')', ') ')

    s_split = s.split()
    s = ''

    for it in range(len(s_split)):
        if funcs.get(s_split[it]) is not None:
            s_split[it] = funcs.get(s_split[it])

        s += s_split[it] + ' '

    while s.count('( ') != 0:
        s = s.replace('( ', '(')
    while s.find(' (') != -1:
        s = s.replace(' (', '(')
    while s.find(') ') != -1:
        s = s.replace(') ', ')')
    while s.find('( ') != -1:
        s = s.replace('( ', '(')
    while s.find(' )') != -1:
        s = s.replace(' )', ')')

    return s


# open file with funcs


try:
    open('funcs_list.txt')
except BaseException:
    error("couldn't find file with funcs")

funcs_lines = open('funcs_list.txt').readlines()
funcs = {}

for line in funcs_lines:
    tmp = line.split(' - ')
    funcs[tmp[1].split('\n')[0]] = tmp[0]


# open config file


try:
    open('config.txt')
except BaseException:
    error("couldn't find config file")

config = {}
config_lines = open('config.txt').readlines()

for line in config_lines:
    line_s = line.split(' = ')
    config[line_s[0]] = line_s[1].split('\n')[0]


# open file with code


if config.get('read_path') is not None:
    path_to_file = config['read_path']
else:
    path_to_file = input('path to the file: \n')

try:
    open(path_to_file)
except BaseException:
    error("couldn't find such file")


lines = open(path_to_file).readlines()


# create output file


filename = path_to_file.split('\\')[-1].split('.')[0]
filename += '.sb'

if config.get('save_path') is not None:
    path_to_file = config['save_path']
else:
    path_to_file = input('where do you want to save output file: \n')

try:
    open(path_to_file+'\\'+filename, 'r')
    if config.get('warning_displaying') == '1' or config.get('warning_displaying') is None:
        print("warning: file with such name already exists")
        print("this file will be deleted, press any key if you want to continue")
        input()
        print("deleting...")
        print("creating " + filename + " in the selected folder...")
    os.remove(path_to_file+'\\'+filename)
except BaseException:
    print("creating " + filename + " in the selected folder...")

output = open(path_to_file+'\\'+filename, 'w')
output_lines = []


# main part


lstring = ''
for line in lines:
    lstring += line + '\n'

unchanged_c = 0
unchanged = []
lstring_s = lstring.split('"')
for i in range(len(lstring_s)):
    if i % 2 == 1:
        unchanged.append(lstring_s[i])
        lstring_s[i] = '#' + str(unchanged_c) + '#'
        unchanged_c += 1

lstring = ''
for i in lstring_s:
    lstring += i + '"'

lstring = lstring[:-1]
lstringc = lstring
lstring = ''
write_bool = 1

for i in range(len(lstringc)):
    if i != range(len(lstringc))[-1] and lstringc[i] + lstringc[i+1] == '/*':
        write_bool = 0
    if i != range(len(lstringc))[-1] and lstringc[i] + lstringc[i + 1] == '*/':
        write_bool = 1
    if write_bool:
        lstring += lstringc[i]

lstring = lstring.replace('/*', '')
lstring = lstring.replace('*/', '')

lines = []
lstring_s = lstring.split('\n')
for i in lstring_s:
    lines.append(i)


again = 1
flag = 0
while again:
    for i in range(len(lines)):
        tmp = lines[i].split()
        tmp += '\n'
        if tmp[0] == '{' and i - 1 >= 0:
            lines[i-1] += '{'
            lines[i] = lines[i].replace('{', ' ')
            flag = 1
            continue
        if tmp[0] == 'else' and i - 1 >= 0:
            lines[i-1] += ' ' + lines[i]
            lines[i] = ''
            flag = 1
    if flag:
        again = 1
    else:
        again = 0
    flag = 0

close = []
return_type = ''
func_props = {}
local_vars = []
i = 0
for line in lines:
    print(line)
    i += 1
    output_line = line

    while output_line.find('  ') != -1:
        output_line = output_line.replace('  ', ' ')

    if output_line.find('//') != -1:
        output_line = output_line.split('//')[0]

    try:
        if output_line[-1] == ';':
            output_line = output_line[:-1]+'\n'

        if output_line[-2] == ';':
            output_line = output_line[:-2]+'\n'

        if output_line[-3] == ';':
            output_line = output_line[:-3]+'\n'
    except IndexError:
        pass

    tmp = output_line.split()
    tmp.append('\n')
    fw = tmp[0].lower()

    if fw == 'if':
        check_start()
        output_line = output_line.replace('==', '=')
        output_line = output_line.replace('!=', '<>')
        st = output_line.find('(')
        if st == -1:
            error("couldn't find '(' in the line " + str(i))
        end = output_line.rfind(')')
        if end == -1:
            error("couldn't find ')' in the line " + str(i))
        case_part = output_line[st + 1:end]
        output_line = 'if ' + case_part + ' then'
        tmp_tuple = ('endif', i)
        close.append(tmp_tuple)
    elif fw == 'for':
        check_start()
        st = output_line.find('(')
        if st == -1:
            error("couldn't find '(' in the line " + str(i))
        end = output_line.rfind(')')
        if end == -1:
            error("couldn't find ')' in the line " + str(i))
        case_part = output_line[st+1:end]
        if case_part.count(';') != 2:
            error("found " + str(case_part.count(';')) + " semicolons instead of 2 in the line " + str(i))
        c_list = case_part.split(';')
        c_var = c_list[0]
        c_stop = c_list[1]
        c_act = c_list[2]
        if c_act.find('++') != -1:
            c_act = c_var.split('=')[0].replace(' ', '') + ' = ' + c_var.split('=')[0].replace(' ', '') + ' + 1'
        if c_act.find('--') != -1:
            c_act = c_var.split('=')[0].replace(' ', '') + ' = ' + c_var.split('=')[0].replace(' ', '') + ' - 1'
        if c_act.find('+=') != -1:
            c_act = c_act.split('+=')[0] + ' = ' + c_act.split('+=')[0] + ' + ' + c_act.split('+=')[1]
        if c_act.find('-=') != -1:
            c_act = c_act.split('-=')[0] + ' = ' + c_act.split('+=')[0] + ' - ' + c_act.split('-=')[1]
        c_var = change_defined_funcs(c_var)
        c_var += '\n'
        output_lines.append(c_var)
        output_line = 'while ' + c_stop
        tmp_tuple = (c_act + '\nendwhile', i)
        close.append(tmp_tuple)
    elif fw == 'while':
        check_start()
        output_line = output_line.replace('==', '=')
        output_line = output_line.replace('!=', '<>')
        st = output_line.find('(')
        if st == -1:
            error("couldn't find '(' in the line " + str(i))
        end = output_line.rfind(')')
        if end == -1:
            error("couldn't find ')' in the line " + str(i))
        case_part = output_line[st + 1:end]
        case_part_s = case_part.split()
        case_part = ''
        for j in case_part_s:
            if j == 'true':
                case_part += '"True" '
            else:
                case_part += j + ' '
        output_line = 'while ' + case_part
        tmp_tuple = ('endwhile', i)
        close.append(tmp_tuple)
    elif fw == 'void':
        check_start()
        output_line = 'sub ' + output_line.replace('{', '')[5:]
        tmp_tuple = ('endsub', i)
        close.append(tmp_tuple)
    elif fw == 'func':
        check_start()

        output_line = output_line.replace('{', '')
        st = output_line.find('(')
        if st == -1:
            error("couldn't find '(' in the line " + str(i))
        end = output_line.rfind(')')
        if end == -1:
            error("couldn't find ')' in the line " + str(i))
        args = output_line[st + 1:end]
        output_line = output_line.replace('(' + args + ')', '')
        output_line = output_line.replace('{', '')

        func_type = output_line.split()[1]
        if func_type != 'str' and func_type != 'num':
            error('it appears that you did not define the type of the function properly in the line '+str(i))

        func_name = output_line.split()[2]
        if func_props.get(func_name) is not None:
            error('defining the function that is already defined in the line '+str(i))

        output_line = 'sub ' + output_line[9:]

        args_s = args.split(',')
        try:
            for j in range(len(args_s)):
                args_s[j] = args_s[j].replace('=', ' = ')
                args_s[j] = args_s[j].replace('"', '')
                while args_s[j].find('  ') != -1:
                    args_s[j] = args_s[j].replace('  ', ' ')
                s_tmp = args_s[j].split()
                if len(s_tmp) > 1 and s_tmp[1] == '=':
                    error("incorrect format of parameter in the line "+str(i))
                local_vars.append(s_tmp[0])
            args = ''
            for arg in args_s:
                args += arg+','
            args = args[0:-1]
            args_s = args.split()
            args = ''
            for arg in args_s:
                args += arg
            args = args.replace(',', ' ')
            args_count = args.count(' ') + 1
        except BaseException:
            args_count = 0
        return_type = func_type
        output_lines.append('f.start = ' + func_name + '\n')
        if args != ' ':
            output_lines.append('f.function("'+func_name+'", "'+args+'") \n')
        else:
            output_lines.append('f.function("'+func_name+'", "") \n')
        output_lines.append(output_line+'\n')
        func_props[func_name] = (args_count, len(output_lines)-1)
        tmp_tuple = ('endsub', i)
        close.append(tmp_tuple)
        continue
    elif fw == 'return':
        if return_type == '':
            error('unexpected return in the line '+str(i))
        val = output_line[7:].split('\n')[0]
        if return_type == 'num':
            output_line = 'f.returnnumber(' + val + ')'
        else:
            output_line = 'f.returntext(' + val + ')'

        while output_line.find('( ') != -1:
            output_line = output_line.replace('( ', '(')
        while output_line.find(' )') != -1:
            output_line = output_line.replace(' )', ')')
        output_line = output_line.replace('f.returnnumber()', 'f.return()')
        output_line = output_line.replace('f.returntext()', 'f.return()')
    elif fw == 'local':
        local_index = -1
        for k in range(i):
            if close.count(('endsub', k)) != 0 and lines[k-1].lower().split()[0] == 'func':
                local_index = k
        if local_index == -1:
            error("defining local variable out of the function in the line "+str(i))
        tmp_args, main_index = func_props[lines[local_index-1].replace('(', ' (').split()[2]]
        output_line = output_line.replace('=', ' = ')
        while output_line.find('  ') != -1:
            output_line = output_line.replace('  ', ' ')
        local_s = output_line.split()
        local_s.pop(0)

        if len(local_s) > 1:
            local_str = local_s[0]+':'+local_s[2]
        else:
            local_str = local_s[0]

        local_vars.append(local_s[0])

        if output_lines[main_index-1].replace(') ', ')')[-4] != '"':
            output_lines[main_index-1] = output_lines[main_index-1].replace(') ', ')')[:-3] + ' ' + local_str + '")\n'
        else:
            output_lines[main_index-1] = output_lines[main_index-1].replace(') ', ')')[:-3] + local_str + '")\n'

        output_line = ''

    local_clean = 1
    for k in range(i):
        if close.count(('endsub', k)) != 0:
            local_clean = 0
    if local_clean == 1:
        local_vars = []

    output_line = output_line.replace('=', ' = ')
    while output_line.find('  ') != -1:
        output_line = output_line.replace('  ', ' ')

    tmp = output_line.split()
    tmp.append('\n')

    if len(tmp) > 1 and local_vars.count(tmp[0]) != 0 and tmp[1] == '=':
        tstr = ''
        for kt in tmp[1:]:
            tstr += kt + ' '
        output_line = 'f.set("'+tmp[0]+'", '+tstr.replace('= ', '')+')'

    output_line = change_defined_funcs(output_line)

    if output_line.find('}') != -1:
        if len(close) < 1:
            error("couldn't find matching opening operator for the '}' in the line " + str(i))
        matching_word, line = close.pop()
        if output_line.find('else') != -1:
            tmp_tuple = ('endif', i)
            close.append(tmp_tuple)
            output_line = 'else'
        else:
            output_line = matching_word

    output_line += '\n'
    output_line = output_line.replace('> =', '>=')
    output_line = output_line.replace('< =', '<=')

    output_lines.append(output_line)

if len(close) > 0:
    matching_word, line = close.pop()
    error("missing matching closing operator for the '{' in the line " + str(line))


# writing to the file


for line in output_lines:
    for k in range(len(unchanged)):
        line = line.replace('#'+str(k)+"#", unchanged[k])
    if line != '\n' and line != '':
        output.write(line)


# end


if config.get('waiting_after') == '1' or config.get('waiting_after') is None:
    input('press any key to complete')
    exit(0)
