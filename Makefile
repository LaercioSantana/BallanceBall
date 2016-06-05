CC = g++
OPENCV_LIB = `pkg-config --cflags --libs opencv`
OTHER_LIBS = -larmadillo
CFLAGS = -w -g -O0 -std=c++11
EXECUTABLE = ./bin/app
MAIN = ./main.cpp
SOURCES = ./Camera.cpp \
		  ./Util.cpp \
		  ./Serial.cpp\
		  ./DynamicSystem.cpp\
		  ./SerialSystem.cpp\
		  ./GainSystem.cpp\
		  ./DifferentialEquation.cpp\
		  ./BallanceBallPlant.cpp\
		  ./CloseLoop.cpp\
		  ./ParallelSystem.cpp\
		  ./PID.cpp\

OBJECTS_PATH = ./obj
OBJECTS_TEMP = $(SOURCES:./%=$(OBJECTS_PATH)/%)
OBJECTS = $(OBJECTS_TEMP:%.cpp=%.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(OBJECTS_PATH)/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(OPENCV_LIB) $(OTHER_LIBS) 

$(OBJECTS_PATH)/%.o: ./%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

force:
	touch main.cpp
	make

clean:
	rm $(OBJECTS_PATH)/*.o

run:
	make
	./$(EXECUTABLE)

test: $(OBJECTS) $(OBJECTS_PATH)/test.o
	$(CC) $(CFLAGS) -o ./bin/$@ $^ $(OPENCV_LIB) $(OTHER_LIBS)

capture: $(OBJECTS) $(OBJECTS_PATH)/captureData.o
	$(CC) $(CFLAGS) -o ./bin/$@ $^ $(OPENCV_LIB) $(OTHER_LIBS)	

