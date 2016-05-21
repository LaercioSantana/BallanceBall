CC = g++
OPENCV_LIB = `pkg-config --cflags --libs opencv`
EXECUTABLE = ./bin/app
SOURCES = ./Camera.cpp \
		  ./Util.cpp \
		  ./main.cpp \

OBJECTS_TEMP = $(SOURCES:./%=./obj/%)
OBJECTS = $(OBJECTS_TEMP:%.cpp=%.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^ $(OPENCV_LIB)

./obj/%.o: ./%.cpp
	$(CC) -c $^ -o $@

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
