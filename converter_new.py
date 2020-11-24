import re
import os
filepath = './data'
files = os.listdir(filepath)
for file in files:
    path = "./data/" + file
    line = []
    with open(path) as f:
        for s_line in f:
            words = re.split(" +", s_line.strip())
            line.append(words)
    length = len(line)


    MaximuRouteDuraion = int(line[0][2])

    outputpath = "./new_revised/"+ file
    outputpath = os.path.abspath(outputpath)
    with open(outputpath, mode='w') as f:
        # 1行目
        string = ""
        for i in range(len(line[0])):
            if i != 0:
                string += ","
            string += line[0][i]
        f.write(string)
        f.write("\n")

        for i in range(1,length-1):
            string = ""  
            # 入力ファイルをそのまま出力
            for j in range(len(line[i])):
                if j != 0:
                    string += ","
                string += line[i][j]
            f.write(string)
            f.write("\n")

            # 区分線形関数にして出力

            item = line[i]
            # 時間枠制限がないもの
            if int(item[-1]) >= MaximuRouteDuraion and int(item[-2]) == 0:
                # 0からMaximuRouteDuraionのあいだで，傾き0 切片0
                pass
                f.write("0,"+str(MaximuRouteDuraion)+",0,0\n")

            # 下限だけに制約あり
            elif int(item[-1]) >= MaximuRouteDuraion:
                pass
                lower = line[i][-2]
                upper = line[i][-1]
                f.write("0,"+lower+",-1,"+lower)
                f.write(","+lower+","+str(MaximuRouteDuraion)+",0,0")
                # f.write(","+str(MaximuRouteDuraion)+","+str(MAX_NUM)+",1,"+"-"+str(MaximuRouteDuraion))
                f.write("\n")

            # 上限だけに制約あり
            elif int(item[-2]) == 0:
                pass
                lower = line[i][-2]
                upper = line[i][-1]
                f.write("0,"+lower+",0,0")
                f.write(","+lower+","+str(MaximuRouteDuraion)+",1,-"+lower)
                f.write("\n")
            
            
            else:
                lower = line[i][-2]
                upper = line[i][-1]
                f.write("0,"+lower+",-1,"+lower)
                f.write(","+lower+","+upper+",0,0")
                f.write(","+upper+","+str(MaximuRouteDuraion)+",1,-"+upper)
                f.write("\n")

