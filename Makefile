CC = g++
OPENCV_LIB = `pkg-config --cflags --libs opencv`
CFLAGS = -w -g -O0
EXECUTABLE = ./bin/app
SOURCES = ./Camera.cpp \
		  ./Util.cpp \
		  ./Serial.cpp\
		  ./main.cpp \

OBJECTS_TEMP = $(SOURCES:./%=./obj/%)
OBJECTS = $(OBJECTS_TEMP:%.cpp=%.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(OPENCV_LIB)

./obj/%.o: ./%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

force:
	touch main.cpp
	make
clean:
	rm ./*.o
run:
	make
	./$(EXECUTABLE)
test:
	@echo $(OBJECTS)
