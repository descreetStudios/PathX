#include "Application.h"
#include "GridLayer.h"

int main()
{
	app::push_ui_layer(new GridLayer(20.0f, 32.0f));

	if (!app::init(1280u, 720u, "PathX Visualizer")) return -1;
	app::run();
	app::shutdown();
}