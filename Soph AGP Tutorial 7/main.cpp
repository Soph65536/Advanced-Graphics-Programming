#include <iostream>
#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"
#include "GameObject.h"
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

	Mesh mesh_cube{ renderer, "Assets/cube.obj" };
	Mesh mesh_sphere{ renderer, "Assets/sphere.obj" };

	GameObject obj1{ "Cube", &mesh_cube };
	GameObject obj2{ "Sphere", &mesh_sphere };

	//set camera  and renderer positions
	renderer.camera.transform.SetPosition({ 0, 0, -1 });

	renderer.RegisterGameObject(&obj1);
	obj1.transform.SetPosition({ -3, 0, 0, 1 });

	renderer.RegisterGameObject(&obj2);
	obj2.transform.SetPosition({ 3, 0, 0, 1 });

	//declare keyboard tracker
	DirectX::Keyboard::KeyboardStateTracker kbTracker;

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
			//get keyboard input state
			auto kbState = DirectX::Keyboard::Get().GetState();
			kbTracker.Update(kbState);

			if (kbTracker.pressed.Escape) {
				PostQuitMessage(0);
			}

			//keyboard camera movement
			if (kbTracker.lastState.W) {
				renderer.camera.transform.Translate({ 0, 0, 0.001f });
			}
			if (kbTracker.lastState.S) {
				renderer.camera.transform.Translate({ 0, 0, -0.001f });
			}
			if (kbTracker.lastState.A) {
				renderer.camera.transform.Translate({ -0.001f, 0, 0 });
			}
			if (kbTracker.lastState.D) {
				renderer.camera.transform.Translate({ 0.001f, 0, 0 });
			}
			if (kbTracker.lastState.Q) {
				renderer.camera.transform.Translate({ 0, 0.001f, 0 });
			}
			if (kbTracker.lastState.E) {
				renderer.camera.transform.Translate({ 0, -0.001f, 0 });
			}

			//get mouse input state
			auto msState = DirectX::Mouse::Get().GetState();

			//mouse camera rotate
			renderer.camera.transform.Rotate({ -(float)msState.y * 0.001f, (float)msState.x * 0.001f, 0 });
			if (msState.leftButton) {
				renderer.camera.transform.SetPosition({ 0, 0, -1 });
			}

			static float fakeTime = 0;
			static float speed = 0.0001f;
			fakeTime += speed;

			obj1.transform.SetPosition({ 3, cos(fakeTime), 5 - sin(fakeTime) });
			obj1.transform.Rotate({ speed, speed, 0 });

			obj2.transform.SetPosition({ -cos(fakeTime) - 3, -sin(fakeTime), 5 });
			obj2.transform.Rotate({ speed, speed, 0 });

			renderer.RenderFrame();
		}
	}

	renderer.Clean();

	return 0;
}