# imports. you may need to sudo apt-get install rosbag, rospy, cv-bridge (all ros packages) and some other stuff
import matplotlib.pyplot as plt
import matplotlib.animation
import IPython.display
import cv2
import cv_bridge
import subprocess
import itertools
import pickle
import os
from copy import copy,deepcopy


bagobj=open("ebagfilename",'rb')
bag=pickle.load(bagobj)
bag=jsonpickle.decode(bag)

tmpobj=open("etmpfilename",'rb')
tmp=pickle.load(tmpobj)
tmp=jsonpickle.decode(tmp)

img=[]
bridge=cv_bridge.CvBridge()
for (topic,msg,t) in itertools.islice((iterator),startframe,endframe): #frame window you want to extract
    img.append(bridge.imgmsg_to_cv2(msg,'mono16'))
img=np.array(img)
#img.shape

# non-background subtracted video
save_file=os.path.splitext(os.path.basename(filename))[0]
# remove the hashtag in the line below to get non-background subtracted mp4
#write_frame_preview(save_file+'raw.mp4',img,depth_min=0,depth_max=1000,camera_fs=framerate)

# background subtraction
img=(np.median(img,0)-img).astype('uint16')
img[img<backgroundmin]=0
img[img>backgroundmax]=0

# mp4 for video with simple background subtraction. If this looks good you can go ahead and do whatever filter you want below
write_frame_preview(save_file+'.mp4',img,depth_min=backgroundmin,depth_max=backgroundmax,camera_fs=framerate)

# median filter
filt_img=deepcopy(img)
for i in range(img.shape[0]):
    filt_img[i,...]=cv2.medianBlur(img[i,...],1) 

write_frame_preview(save_file+'medfilt5px.mp4',filt_img,depth_min=backgroundmin,depth_max=backgroundmax,camera_fs=framerate)

# gaussian filter
filt_img=deepcopy(img)
for i in range(img.shape[0]):
    filt_img[i,...]=cv2.GaussianBlur(img[i,...],(1,1,),0) 
# the larger the ordered pair, the less of the mouse body I see. the last number doesnt seem to do anything

write_frame_preview(save_file+'gaussfilt9px.mp4',filt_img,depth_min=backgroundmin,depth_max=backgroundmax,camera_fs=framerate)




