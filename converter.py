import re
import os
path = 'instance.txt'
path_w = 'output.txt'
filepath = './data'
files = os.listdir(filepath)
for file in files:
    if file != ".DS_Store":
        path = "./data/" + file
        line = []
        with open(path) as f:
            for s_line in f:
                words = re.split(" +", s_line)
                line.append(words)
        print(line)
        outputpath = "./revised/" + file
        outputpath = os.path.abspath(outputpath)
        with open(outputpath, mode='a') as f:
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
