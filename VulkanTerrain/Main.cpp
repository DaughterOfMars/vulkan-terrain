#include "VulkanTerrain.h"

VulkanTerrain * app;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (app != NULL)
	{
		app->meshRenderer->handleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	app = new VulkanTerrain(false);
	app->meshRenderer->winHandle = app->setupWindow(hInstance, WndProc);
	app->initSwapChain();
	app->prepare();
	app->render;
	delete(app);
	return 0;
}