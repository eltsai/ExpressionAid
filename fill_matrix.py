import numpy as np
import string
import os
import glob as gb

landmark_path=gb.glob("Facewarehouse_data/*/TrainingPose/*.txt")
matrix_path="Facewarehouse_data/Matrix_path.txt"
name_path="Facewarehouse_data/name_path.txt"

matrix=open(matrix_path,"a")
name=open(name_path,"a")

for path in landmark_path:
    print path
    name.write('\"'+path+'\"'+','+'\n')
    f=open(path)
    line=f.readline()
    while line:
        line=line[:-1]
        print line
        matrix.write(line+", ")
        line=f.readline()

    matrix.write('\n')
