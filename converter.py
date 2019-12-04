import re
path = 'instance.txt'
path_w = 'output.txt'
line = []
with open(path) as f:
    for s_line in f:
        words = re.split(" +", s_line)
        line.append(words)
print(line)
print(line[2][0])
with open(path_w, mode='a') as f:
    for item in line:
        for subitem in item:
            if  subitem.find(' ') == -1:
                if subitem == "":
                    pass
                elif subitem.find('\n') == -1:
                    str = subitem + ","
                    f.write(str)
                else:
                    f.write(subitem)
                    print('aaa')
