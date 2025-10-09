#include <iostream>
#include "Window.h"
#include "Renderer.h"
#include "Debug.h"

//window main
int WINAPI WinMain(_In_ HINSTANCE instanceH, _In_opt_ HINSTANCE prevInstanceH, _In_ LPSTR lpCmdLine, _In_ int  nCmdShow) {
	//initialise window with error check
	Window window{ 800, 600, instanceH, nCmdShow };
	Renderer renderer{ window };

	//used to hold windows event messages
	MSG msg;

	printf("Hello World\n");
	char letter[64];
	std::cin >> letter;
	std::cout << letter << std::endl;

	//main game loop
	while (true) {
		renderer.RenderFrame();
	}

	renderer.Clean();

	return 0;
}