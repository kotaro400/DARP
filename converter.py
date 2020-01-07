import re
import os
path = 'instance.txt'
path_w = 'output.txt'
filepath = './data'
files = os.listdir(filepath)
for file in files:
    print(file)
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
        print(outputpath)
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
                # print(item)
                if item == ['', '']:
                    break
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
                    f.write("0,0,1,0,2,0,"+str(item[-1].strip("\n"))+",0\n")
                elif int(item[-1].strip("\n")) >= MaximuRouteDuraion:
                    f.write(str(int(item[-2])-1)+",1,"+str(item[-2])+",0,"+str(int(item[-1].strip("\n"))-1)+",0,"+str(item[-1].strip("\n"))+",0\n")
                elif int(item[-2]) == 0:
                    f.write("0,0,1,0,"+str(item[-1].strip("\n"))+",0,"+str(int(item[-1].strip("\n"))+1)+",1\n")
                else:
                    f.write(str(int(item[-2])-1)+",1,"+str(item[-2])+",0,"+str(item[-1].strip("\n"))+",0,"+str(int(item[-1].strip("\n"))+1)+",1\n")