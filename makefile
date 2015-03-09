COMPILER=g++

OUTPUT=pong

SOURCE_FILES=src/main.cpp

EXTRA_TAGS=-I/usr/local/include -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system

default: build
build:
	$(COMPILER) -o $(OUTPUT) $(EXTRA_TAGS) $(SOURCE_FILES)
run:
	./$(OUTPUT)
