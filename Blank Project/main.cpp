#include "../nclgl/window.h"
#include "Renderer.h"

int main() {
	Window w("CSC8502 Tropical Island", 1280, 768, false);
	double previousTime = w.GetTimer()->GetTotalTimeSeconds(); // init FPS variables
	int frameCount = 0;

	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);


	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {

		// Displaying FPS
		double currentTime = w.GetTimer()->GetTotalTimeSeconds();
		frameCount++;
		if (currentTime - previousTime >= 1.0) // If a second has passed
		{
			w.SetTitle("CSC8502 Tropical Island | FPS: " + std::to_string(frameCount));

			frameCount = 0;
			previousTime = currentTime;
		}

		float timestep = w.GetTimer()->GetTimeDeltaSeconds();
		renderer.UpdateScene(timestep);
		renderer.RenderScene();
		renderer.SwapBuffers();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}
	return 0;
}