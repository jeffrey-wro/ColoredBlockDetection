OBJS	= main.o
SOURCE	= src/main.cpp
HEADER	= 
OUT		= openCVTest.out
CC		= g++ 
CFLAGS	= -std=c++11 -pthread
INCLUDE = -I/usr/local/include/opencv4/
FLAGS	= -g -c -Wall -O0 -g3 -std=gnu++11 -fmessage-length=0
LFLAGS 	= -L/usr/local/lib64/ -L/usr/local/lib/ -ldl -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab

all: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -g $(OBJS) -o $(OUT)  $(LFLAGS)

main.o: src/main.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) src/main.cpp

clean:
	rm -f $(OBJS) $(OUT)