#include <iostream>
#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "MaterialLit.h"
#include "GameObject.h"
#include "BoxCollider.h"
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

	//get models and textures
	Mesh mesh_cube{ renderer, "Assets/Models/cube.obj" };
	Mesh mesh_sphere{ renderer, "Assets/Models/sphere.obj" };
	Mesh mesh_fish{ renderer, "Assets/Models/fish.obj" };
	Mesh mesh_grass{ renderer, "Assets/Models/grass.obj", true };
	
	Texture tex_skybox{ renderer, "Assets/Textures/Skybox/skybox02.dds", false, Texture::TextureType::Cubemap };
	Texture tex_box{ renderer, "Assets/Textures/Box.bmp" };
	Texture tex_fish{ renderer, "Assets/Textures/fish_texture.png", true };
	Texture tex_flower{ renderer, "Assets/Textures/flower.png", true };
	
	//make materials
	Material mat_unlit{ "Unlit", renderer, "Compiled Shaders/VertexShader.cso", "Compiled Shaders/FragmentShader.cso", &tex_box };
	MaterialLit mat_lit{ "Lit", renderer, "Compiled Shaders/ReflectiveVShader.cso", "Compiled Shaders/ReflectiveFShader.cso", &tex_box };
	mat_lit.SetReflectionTexture(&tex_skybox);

	//skybox object
	GameObject obj_skybox{ "Skybox", &mesh_cube, &mat_unlit };
	renderer.skyboxObject = &obj_skybox;

	//make gameobjects (render transparent objects last!!)
	GameObject objSphere{ "Sphere", &mesh_sphere, &mat_lit };
	GameObject objFish{ "Fish", &mesh_fish, &mat_unlit };
	GameObject objFlower{ "Flower", &mesh_grass, &mat_unlit };

	//set lighting
	renderer.directionalLights[0] = { DirectX::XMVECTOR{ 0.3f, 0.7f, 0.7f }, { 0, 0.8f, 0.75f }, true };
	renderer.directionalLights[1] = { DirectX::XMVECTOR{ 0.0f, 0.2f, 0.5f }, { 0.8f, 0.75f, 0 }, true };
	renderer.pointLights[0] = { DirectX::XMVECTOR{-4, 1, -1}, {0.85f, 0, 0.85f}, 10, true };
	renderer.pointLights[1] = { DirectX::XMVECTOR{2, -1, -1}, {0, 0.85f, 0.85f}, 20, true };

	//set camera and gameobject positions
	renderer.camera.transform.SetPosition({ 0, 0, -5 });

	renderer.RegisterGameObject(&objSphere);
	objSphere.transform.SetPosition({ 0, 0, 3, 1 });

	renderer.RegisterGameObject(&objFish);
	objFish.transform.SetPosition({ -3, 0, 0, 1 });

	renderer.RegisterGameObject(&objFlower);
	objFlower.transform.SetPosition({ 3, 0, 0, 1 });

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

			//camera collision
			if (BoxCollider::BoxCollision(renderer.camera.transform, objFish.transform)) { renderer.RemoveGameObject(&objFish); }
			if (BoxCollider::BoxCollision(renderer.camera.transform, objFlower.transform)) { renderer.RemoveGameObject(&objFlower); }

			objSphere.transform.Rotate({ -deltaTime * 0.5f, 0, 0 });
			objFish.transform.Rotate({ deltaTime, -deltaTime, 0 });
			objFlower.transform.Rotate({ 0, -deltaTime, 0 });

			renderer.RenderFrame();
		}
	}

	renderer.Clean();

	return 0;
}