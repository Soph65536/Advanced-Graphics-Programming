#include <iostream>
#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Debug.h"

const float walkSpeed = 0.001f;
const float runSpeed = 0.003f;

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

	//get models
	Mesh mesh_cube{ renderer, "Assets/cube.obj" };
	Mesh mesh_sphere{ renderer, "Assets/sphere.obj" };

	//make gameobjects
	GameObject obj1{ "Cube", &mesh_cube };
	GameObject obj2{ "Sphere", &mesh_sphere };

	//set camera and gameobject positions
	renderer.camera.transform.SetPosition({ 0, 0, -5 });

	renderer.RegisterGameObject(&obj1);
	obj1.transform.SetPosition({ -3, 0, 0, 1 });

	renderer.RegisterGameObject(&obj2);
	obj2.transform.SetPosition({ 3, 0, 0, 1 });

	//movement
	float moveSpeed = walkSpeed;

	//declare keyboard tracker
	DirectX::Keyboard::KeyboardStateTracker kbTracker;

	//used to hold windows event messages
	MSG msg;

	//main game loop
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg); //translate certain messages into correct format, namely key presses

			DispatchMessage(&msg); //send the message to the windowproc function

			if (msg.message == WM_QUIT) break; //break out of the loop if get quit message
		}
		else {
			//calculate delta time, i will do properly later
			float deltaTime = 0.001f;

			//get keyboard input state
			auto kbState = DirectX::Keyboard::Get().GetState();
			kbTracker.Update(kbState);

			if (kbTracker.pressed.Escape) {
				PostQuitMessage(0);
			}

			if (kbTracker.pressed.LeftShift) {
				moveSpeed = runSpeed;
			}

			if (kbTracker.released.LeftShift) {
				moveSpeed = walkSpeed;
			}

			//keyboard camera movement
			if (kbTracker.lastState.W) {
				renderer.camera.transform.Translate(DirectX::XMVectorScale(renderer.camera.transform.GetForward(), moveSpeed));
			}
			if (kbTracker.lastState.S) {
				renderer.camera.transform.Translate(DirectX::XMVectorScale(renderer.camera.transform.GetForward(), -moveSpeed));
			}
			if (kbTracker.lastState.A) {
				renderer.camera.transform.Translate(DirectX::XMVectorScale(renderer.camera.transform.GetRight(), -moveSpeed));
			}
			if (kbTracker.lastState.D) {
				renderer.camera.transform.Translate(DirectX::XMVectorScale(renderer.camera.transform.GetRight(), moveSpeed));
			}
			if (kbTracker.lastState.Q) {
				renderer.camera.transform.Translate(DirectX::XMVectorScale(renderer.camera.transform.GetUp(), moveSpeed));
			}
			if (kbTracker.lastState.E) {
				renderer.camera.transform.Translate(DirectX::XMVectorScale(renderer.camera.transform.GetUp(), -moveSpeed));
			}

			//get mouse input state
			auto msState = DirectX::Mouse::Get().GetState();

			//mouse camera rotate
			renderer.camera.transform.Rotate({ -(float)msState.y * 0.001f, (float)msState.x * 0.001f, 0 });
			if (msState.leftButton) {
				renderer.camera.transform.SetPosition({ 0, 0, -5 });
			}

			obj1.transform.Rotate({ deltaTime, deltaTime, 0 });
			obj2.transform.Rotate({ deltaTime, deltaTime, 0 });

			renderer.RenderFrame();
		}
	}

	renderer.Clean();

	return 0;
}