.DEFAULT_GOAL := generate

generate:
	mkdir -p build_linux
	g++ -pthread `pkg-config --cflags glfw3` -fdiagnostics-color=always -g *.cpp imgui/*.cpp -o build_linux/MediaManager -lcurl -lGL `pkg-config --static --libs glfw3` -I imgui --std=c++17
	cp NotoSansJP-Medium.ttf build_linux/NotoSansJP-Medium.ttf
	cp MediaManager.png build_linux/MediaManager.png
	@echo "Build successful"
