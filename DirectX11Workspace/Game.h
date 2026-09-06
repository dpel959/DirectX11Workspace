#pragma once
#include <memory>
#include <windows.h>

class Graphics;
class Pipeline;
class GameObject;

class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

private:
	HWND _hwnd;

	std::shared_ptr<Graphics> _graphics;
	std::shared_ptr<Pipeline> _pipeline;

	std::shared_ptr<GameObject> _parentObject;
	std::shared_ptr<GameObject> _childObject;
};

