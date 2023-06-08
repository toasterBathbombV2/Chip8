test: clean
	clear
	@echo "Compiling and Running Test"
	g++ -std=c++17 main.cpp -I"include" -L"lib" -Wall -lSDL2main -lSDL2 -lSDL2_image -o main.o
	./main.o

clean:
	@echo "Removing Test executable"
	rm -f ./main.o