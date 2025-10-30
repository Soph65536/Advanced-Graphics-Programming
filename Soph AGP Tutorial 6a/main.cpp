#include <iostream>
#include "Window.h"
#include "Renderer.h"
#include "Debug.h"

//console debug stuff
void OpenConsole() {
	if (AllocConsole()) {
		//redirecting input, output and errors to allocated console
		FILE* fp = nullptr;
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		std::ios::sync_with_stdio(true);

		std::cout << "Hello Side Console! :)" << std::endl;
	}
}

//window main
int WINAPI WinMain(_In_ HINSTANCE instanceH, _In_opt_ HINSTANCE prevInstanceH, _In_ LPSTR lpCmdLine, _In_ int  nCmdShow) {
	//initialise window with error check
	Window window{ 800, 600, instanceH, nCmdShow };
	Renderer renderer{ window };

	window.SetCamera(renderer.camera);

	//set camera  and renderer positions
	renderer.camera.transform.SetPosition({ 0, 0, -1 });
	renderer.transform1.SetPosition({ 0, 0, 1 });

	//used to hold windows event messages
	MSG msg;

	printf("Hello World\n");
	char letter[64];
	std::cin >> letter;
	std::cout << letter << std::endl;

	//main game loop
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg); //translate certain messages into correct format, namely key presses

			DispatchMessage(&msg); //send the message to the windowproc function

			if (msg.message == WM_QUIT) break; //break out of the loop if get quit message
		}
		else {
			static float fakeTime = 0;
			static float speed = 0.0001f;
			fakeTime += speed;
			renderer.transform1.SetPosition({ 0, cos(fakeTime), 5 - sin(fakeTime) });
			renderer.transform1.Rotate({ speed, speed, 0 });
			renderer.transform2.SetPosition({ -cos(fakeTime), -sin(fakeTime), 5 });
			renderer.transform2.Rotate({ speed, speed, 0 });

			renderer.RenderFrame();
		}
	}

	renderer.Clean();

	return 0;
}