import numpy as np
import string
import os
import shutil
import glob as gb
import cv2

#name_path=gb.glob("Facewarehouse_data/*/TrainingPose/*.txt")
name_path="Facewarehouse_data/name_path.txt"
label_path="Facewarehouse_data/labels.txt"


name=open(name_path,"r")
label=open(label_path,"r")

i=1

while 1:
    name_line=name.readline()
    label_line=label.readline()
    if not name_line:
        break
    print name_line
    name_line=name_line[1:-6]+"png"
    print name_line
    print label_line
    label_line=label_line[:-1]
    label_line="Facewarehouse_data/"+label_line+"/"
    folder=os.path.dirname(label_line)
    if not os.path.exists(folder):
        os.makedirs(folder)
    label_line=label_line+str(i)+name_line[-4:]
    print label_line

    shutil.copy(name_line,label_line)

    i=i+1

    #img=cv2.imread(name_line)
    #cv2.imshow('img',img)
    #cv2.waitKey(0)
