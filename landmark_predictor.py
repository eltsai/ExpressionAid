import cv2
import dlib
import numpy as np
import string
import os
import glob as gb

#cap = cv2.VideoCapture(0)
#cap.isOpened()
#cap = cv2.VideoCapture(0)
#cap.isOpened()


def ket_points(img,path):

    PREDICTOR_PATH = "shape_predictor_68_face_landmarks.dat"

    detector = dlib.get_frontal_face_detector()
    predictor = dlib.shape_predictor(PREDICTOR_PATH)
    rects = detector(img,1)
    points_keys = []
    File=open(path,"w")

    for identity,d in enumerate(rects):
        pos = (d.left(),d.top())
        cv2.circle(img,pos,2,(255,0,0),-1)

        pos = (d.right(),d.bottom())
        cv2.circle(img,pos,2,(255,0,0),-1)

        sub_x=d.left()-d.right()
        sub_y=d.bottom()-d.top()
        print sub_x
        print sub_y

        for i in range(len(rects)):
            landmarks = np.matrix([[p.x,p.y] for p in predictor(img,rects[i]).parts()])
            img = img.copy()
            for idx,point in enumerate(landmarks):
                pos = (point[0,0],point[0,1])
                align_x=(point[0,0]-d.right())*100/sub_x
                align_y=(point[0,1]-d.top())*100/sub_y
                File.write(str(align_x)+"\n")
                print align_x
                File.write(str(align_y)+"\n")
                print align_y
                points_keys.append(pos)
                cv2.circle(img,pos,2,(255,0,0),-1)
    return img

img_path = gb.glob("Facewarehouse_data/*/TrainingPose/*.png")
for path in img_path:
    print path
    img=cv2.imread(path)
    #cv2.imshow('img',img)
    path=path[:-4]
    path=path+".txt"
    print path

    landmark_detected=ket_points(img,path)
    #cv2.imshow('landmark_detected',landmark_detected)
    #cv2.waitKey(1000)
    #cv2.destroyAllWindows()

#f=open("Facewarehouse_data/list.txt")
#line=f.readline()
#while line:
#    print line
#    #img_file="Facewarehouse_data"+line
#    #print img_file
#    #img_file[:-1]
#    #print img_file
#    line="/home/chern/"+line
#    print line
#    img=cv2.imread(line)
#    #face_key=ket_points(img)
#    cv2.imshow('frame',img)
#    cv2.waitKey(0)
#
#    line=f.readline()
#
#f.close()
#while(True):
#    ret, frame = cap.read()
#    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
#    #face_key = ket_points(gray)
#    cv2.imshow('frame',frame)
#    #cv2.waitKey(0)
#    if cv2.waitKey(1) & 0xFF == ord('q'):
#        break
#
#cap.release()
#cv2.destroyAllWindows()

    #file = open('Facewarehouse_data/list.txt')
    #while 1:
        #line=file.readline()
        #print line
        #imgname='Facewarehouse_data/'+line
        #print imgname
        #img=cv2.imread(imgname)
#img=cv2.imread('Facewarehouse_data/Tester_100/TrainingPose/pose_0.png')
##cv2.imshow('img',img)
##ret, frame = cap.read()
##frame=cv2.resize(img,(192,108))
##gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
#face_key = ket_points(img)
#cv2.imshow('img',face_key)
#cv2.destroyAllWindows()
    #if cv2.waitKey(1) & 0xFF == ord('q'):
    #    break

#cap.release()
