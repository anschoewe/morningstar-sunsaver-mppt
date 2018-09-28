all: sunsaver.cpp
	g++ `pkg-config --cflags --libs libmodbus` sunsaver.cpp -o sunsaver && chmod +x sunsaver
run: sunsaver
	./sunsaver
clean: sunsaver
	rm sunsaver
