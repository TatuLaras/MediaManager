.DEFAULT_GOAL := generate

generate:
	mkdir -p build_linux
	
	g++ `pkg-config --cflags glfw3` -fdiagnostics-color=always -g *.cpp imgui/*.cpp -o build_linux/MediaManager -lcurl -lGL `pkg-config --static --libs glfw3` -I imgui
	cp NotoSansJP-Medium.ttf build_linux/NotoSansJP-Medium.ttf
	@echo "Build successful"
