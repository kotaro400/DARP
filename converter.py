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
        # print(line)
        MaximuRouteDuraion = int(line[0][2])
        outputpath = "./revised/" + file
        outputpath = os.path.abspath(outputpath)
        with open(outputpath, mode='w') as f:
            for subitem in line[0]:
                if  subitem.find(' ') == -1:
                        if subitem == "":
                            pass
                        elif subitem.find('\n') == -1:
                            stri = subitem + ","
                            f.write(stri)
                        else:
                            f.write(subitem)
            line.pop(0)
            for item in line:
                print(item)
                if item == ['', '']:
                    exit(1)
                for subitem in item:
                    if  subitem.find(' ') == -1:
                        if subitem == "":
                            pass
                        elif subitem.find('\n') == -1:
                            stri = subitem + ","
                            f.write(stri)
                        else:
                            f.write(subitem)
                if (int(item[-1].strip("\n")) >= MaximuRouteDuraion) and (int(item[-2]) == 0):
                    f.write("0,0,0"+"\n")
                elif int(item[-1].strip("\n")) >= MaximuRouteDuraion:
                    f.write("0,-5,"+str(int(item[-2])*5)+","+item[-2]+",0,0\n")
                elif int(item[-2]) == 0:
                    f.write("0,0,0,"+item[-1]+",5,"+str(int(item[-1].strip("\n"))*-5)+"\n")
                else:
                    f.write("0,-5,"+str(int(item[-2])*5)+","+item[-2]+",0,0,"+item[-1].strip("\n")+",5,"+str(int(item[-1].strip("\n"))*-5)+"\n")