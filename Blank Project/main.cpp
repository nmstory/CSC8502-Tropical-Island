#include "../nclgl/window.h"
#include "Renderer.h"

// https://cpetry.github.io/TextureGenerator-Online/
//https://www.cleanpng.com/png-skybox-texture-mapping-cube-mapping-sky-cloud-920475/
// https://3dtextures.me/2017/12/28/water-001/

//https://catlikecoding.com/unity/tutorials/flow/waves/
//http://fire-face.com/personal/water/

int main() {
	Window w("CSC8502 Tropical Island", 1280, 720, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	//w.LockMouseToWindow(true);
	//w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
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