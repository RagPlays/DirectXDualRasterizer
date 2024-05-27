#include "pch.h"

#if defined (_DEBUG) | (DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"

using namespace dae;

int main()
{
	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	constexpr uint32_t width{ 640 };
	constexpr uint32_t height{ 480 };

	SDL_Window* pWindow{ 
		SDL_CreateWindow(
			"DirectX - ***Maurice Vandenheede - 2DAE18***",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width, height, 0)
	};

	if (!pWindow) return 1;

	//Initialize "framework"
	std::unique_ptr<Timer> pTimer{ std::make_unique<Timer>() };
	std::unique_ptr<Renderer> pRenderer{ std::make_unique<Renderer>(pWindow, width, height) };

	//Start loop
	pTimer->Start();

	float printTimer{};
	bool showFPS{ true };
	bool isLooping{ true };
	bool clearConsole{ false };

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:	
				isLooping = false;
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.scancode)
				{
				case SDL_SCANCODE_F4:
					pRenderer->ToggleFilteringMode();
					break;
				case SDL_SCANCODE_F5:
					pRenderer->ToggleRotating();
					break;
				case SDL_SCANCODE_F6:
					pRenderer->ToggleNormalMap();
					break;
				case SDL_SCANCODE_F7:
					pRenderer->ToggleFireFX();
					break;
				case SDL_SCANCODE_C:
					clearConsole = true;
					break;
				case SDL_SCANCODE_F:
					showFPS = !showFPS;
					break;
				default:
					break;
				}
				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer.get());

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();

		if (showFPS)
		{
			if (clearConsole) { std::cout << "\x1B[2J\x1B[H"; }
			else
			{
				printTimer += pTimer->GetElapsed();
				if (printTimer >= 1.f)
				{
					printTimer = 0.f;
					std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
				}
			}
		}
	}

	pTimer->Stop();

	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}