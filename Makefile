TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f ./mynewdb.db -n
	./$(TARGET) -f ./mynewdb.db -a "moise,medici,29"
	./$(TARGET) -f ./mynewdb.db -a "aldo,baglio,102"
	./$(TARGET) -f ./mynewdb.db -a "ayeye,brazorf,65"
	./$(TARGET) -f ./mynewdb.db -l

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude

