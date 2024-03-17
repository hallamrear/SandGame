// SandGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <SDL.h>
#undef main

//#define WHITE 0xFFFFFFFF
//#define RED 0xFF0000FF
//#define GREEN 0x00FF00FF
//#define BLUE 0x0000FFFF
//#define BLACK 0x00000000
//#define PINK 0xFF00FFFF
//#define CYAN 0x00FFFFFF
//#define YELLOW 0xFFFF00FF

#define WHITE 0xFFFFFFFF
#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define BLACK 0x00000000
#define PINK 0xFFFF00FF
#define CYAN 0xFFFFFF00
#define YELLOW 0xFF00FFFF

typedef unsigned int Cell;

unsigned int ColourList[8] =
{
    BLACK,
    WHITE,
    RED,
    GREEN,
    BLUE,
    PINK,
    CYAN,
    YELLOW
};

std::string ColourNameList[8] =
{
    "BLACK",
    "WHITE",
    "RED",
    "GREEN",
    "BLUE",
    "PINK",
    "CYAN",
    "YELLOW",
};

int SelectedColour = ColourList[1];
int SpraySize = 15;
int MouseX = 0;
int MouseY = 0;
constexpr int SpraySizeMin = 2;
constexpr int SpraySizeMax = 100;
constexpr int GridWidth  = 800;
constexpr int GridHeight = 800;
constexpr int WindowWidth = 800;
constexpr int WindowHeight = 800;
constexpr int GridArea = GridWidth * GridHeight;

bool IsRunning = false;
bool IsPaused = false;
bool MouseButtonStates[3] = { false, false, false };

SDL_Renderer* Renderer = nullptr;
SDL_Window* Window = nullptr;
SDL_Texture* CellTexture = nullptr;

Cell Cells[GridArea] = { 0 };

SDL_Colour ColourToSDLColour(const unsigned int& colour)
{
    SDL_Colour sdlColour{};
    sdlColour.r = (colour & 0x000000FF);
    sdlColour.g = (colour & 0x0000FF00) >> 8;
    sdlColour.b = (colour & 0x00FF0000) >> 16;
    sdlColour.a = (colour & 0xFF000000) >> 24;
    return sdlColour;
}

bool CanSpawnOnCell(const int& x, const int& y)
{
    int index = (y * GridWidth) + x;

    switch (Cells[index])
    {
        case BLACK:
        case WHITE:
        case GREEN:
        case BLUE:
        case PINK:
        case YELLOW:
        case CYAN:
        {
            return true;
        }
        break;

        case RED:
        default:
        {
            return false;
        }
            break;
    }

    return false;
}

void CreateCell(const int& x, const int& y, const unsigned int& colour)
{
    if (x < 0 || x > GridWidth)
        return;

    if (y < 0 || y > GridHeight)
        return;

    int index = (y * GridWidth) + x;

    int c = rand() % 8;
    
    if (CanSpawnOnCell(x, y) || colour == BLACK)
    {
        Cells[index] = colour;
    }
}

void CreateCellAroundPoint(const int& x, const int& y, const int& radius, const unsigned int& colour)
{
    for (size_t i = 0; i < 5; i++)
    {
        int offset_x = rand() % (radius * 2);
        int offset_y = rand() % (radius * 2);
        int pos_x = x + (offset_x - radius);
        int pos_y = y + (offset_y - radius);
        CreateCell(pos_x, pos_y, colour);
    }
}

void CreateCellSquare(const int& x, const int& y, const int& size, const unsigned int& colour)
{
    for (size_t i = 0; i < size * size; i++)
    {
        int offset_x = i % size;
        int offset_y = i / size;

        int pos_x = x + (offset_x - (size / 2));
        int pos_y = y + (offset_y - (size / 2));

        CreateCell(pos_x, pos_y, colour);
    }
}

void Update()
{
    //Deal with SDL events.
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
        case SDL_QUIT:
        {
            IsRunning = false;
        }
        break;

        case SDL_KEYDOWN:
        {
            switch (sdlEvent.key.keysym.sym)
            {
            case SDLK_1: { SelectedColour = 1; std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;
            case SDLK_2: { SelectedColour = 2; std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;
            case SDLK_3: { SelectedColour = 3; std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;
            case SDLK_4: { SelectedColour = 4; std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;
            case SDLK_5: { SelectedColour = 5; std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;
            case SDLK_6: { SelectedColour = 6; std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;
            case SDLK_7: { SelectedColour = 7; std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;
            //case SDLK_8: { SelectedColour = 8;  std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;
            //case SDLK_9: { SelectedColour = 9;  std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;
            case SDLK_0: { SelectedColour = 0;  std::cout << "Colour selected: " + ColourNameList[SelectedColour] << std::endl; } break;



            case SDLK_SPACE:
            {
                IsPaused = !IsPaused;
            }
            break;

            case SDLK_r:
            {
                //Clear the grid.
                memset(Cells, 0, sizeof(Cell) * GridArea);

                CreateCellSquare((WindowWidth / 2), (WindowHeight / 4), 200, WHITE);
            }
            break;

            default:
                break;
            }
        }
        break;

        case SDL_MOUSEWHEEL:
        {
            if (sdlEvent.wheel.y > 0) // scroll up
            {
                SpraySize++;

                if (SpraySize > SpraySizeMax)
                {
                    SpraySize = SpraySizeMax;
                }

                std::cout << "Spray size: " << SpraySize << std::endl;
            }
            else if (sdlEvent.wheel.y < 0) // scroll down
            {
                SpraySize--;

                if (SpraySize < SpraySizeMin)
                {
                    SpraySize = SpraySizeMin;
                }

                std::cout << "Spray size: " << SpraySize << std::endl;
            }
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            switch (sdlEvent.button.button)
            {
            case SDL_BUTTON_LEFT: { MouseButtonStates[0] = true; } break;
            case SDL_BUTTON_MIDDLE: { MouseButtonStates[1] = true; } break;
            case SDL_BUTTON_RIGHT: { MouseButtonStates[2] = true; } break;

            default:
                break;
            }
        }
        break;

        case SDL_MOUSEBUTTONUP:
        {
            switch (sdlEvent.button.button)
            {
            case SDL_BUTTON_LEFT: { MouseButtonStates[0] = false; } break;
            case SDL_BUTTON_MIDDLE: { MouseButtonStates[1] = false; } break;
            case SDL_BUTTON_RIGHT: { MouseButtonStates[2] = false; } break;

            default:
                break;
            }
        }
        break;

        }
    }

    SDL_GetMouseState(&MouseX, &MouseY);

    if (MouseButtonStates[0])
    {
        CreateCellAroundPoint(MouseX, MouseY, SpraySize / 2, ColourList[SelectedColour]);
    } 
   
    if (MouseButtonStates[2])
    {
        CreateCellSquare(MouseX, MouseY, SpraySize, ColourList[SelectedColour]);
    }

    if (IsPaused == false)
    {
        unsigned int ga = (GridArea - 1);
        for (int i = ga; i != 0; i--)
        {
            Cell& cell = Cells[i];

            int x = i % GridWidth;
            int y = i / GridWidth;

            switch (cell)
            {
            case BLACK:
            case RED:
            {
                Cells[i] = cell;
            }
            break;

            default:
            case GREEN:
            case WHITE:
            {
                //if tile below is filled
                int belowIndex = ((y + 1) * GridWidth) + (x);

                if (belowIndex >= GridArea || belowIndex <= 0) {
                    Cells[i] = cell;
                    continue;
                }

                //If the cell below is occupied, check around it.
                Cell& belowCell = Cells[belowIndex];
                if (belowCell != BLACK)
                {
                    int leftIndex = ((y + 1) * GridWidth) + (x + 1);
                    int rightIndex = ((y + 1) * GridWidth) + (x - 1);

                    if (x + 1 > GridWidth || x - 1 < 0)
                    {
                        Cells[i] = cell;
                        continue;
                    }

                    if (leftIndex >= GridArea || rightIndex >= GridArea)
                    {
                        Cells[i] = cell;
                        continue;
                    }

                    if (leftIndex < 0 || rightIndex < 0)
                    {
                        Cells[i] = cell;
                        continue;
                    }

                    //check below left and below right
                    bool leftEmpty = (Cells[leftIndex] == BLACK);
                    bool rightEmpty = (Cells[rightIndex] == BLACK);

                    if (leftEmpty && rightEmpty)
                    {
                        //Randomly choose which way to go;
                        int r = (rand() % 100) + 1;

                        if (r < 50)
                        {
                            Cells[leftIndex] = cell;
                            Cells[i] = BLACK;
                        }
                        else
                        {
                            Cells[rightIndex] = cell;
                            Cells[i] = BLACK;
                        }
                    }
                    else
                    {
                        if (leftEmpty)
                        {
                            Cells[leftIndex] = cell;
                            Cells[i] = BLACK;
                        }
                        else if(rightEmpty)
                        {
                            Cells[rightIndex] = cell;
                            Cells[i] = BLACK;
                        }
                    }

                }
                else //if the cell below isnt occupied (cell == black)
                {
                    Cells[belowIndex] = cell;
                    Cells[i] = BLACK;
                }
            }
            break;
            }
        }

        //Updating texture memory with the new cell data.
        SDL_UpdateTexture(CellTexture, NULL, Cells, sizeof(SDL_Color) * GridWidth);
    }
}

void Render()
{
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
    SDL_RenderClear(Renderer);

    //Copy the Cells Texture to the screen
    SDL_RenderCopy(Renderer, CellTexture, NULL, NULL);

    //Draw mouse outline rect;
    SDL_Rect drawRect{};
    drawRect.x = MouseX - (SpraySize / 2);
    drawRect.y = MouseY - (SpraySize / 2);
    drawRect.w = SpraySize;
    drawRect.h = SpraySize;

    SDL_Colour drawColour = ColourToSDLColour(ColourList[SelectedColour]);

    SDL_SetRenderDrawColor(Renderer, drawColour.r, drawColour.g, drawColour.b, 255);
    SDL_RenderDrawRect(Renderer, &drawRect);

    SDL_RenderPresent(Renderer);
}

void Create()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_WindowFlags windowFlags = SDL_WindowFlags::SDL_WINDOW_SHOWN;
    Window = SDL_CreateWindow("Sand Game", 128, 128, WindowWidth, WindowHeight, windowFlags);
    if (Window == nullptr)
        return;

    SDL_RendererFlags flags = SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC;
    Renderer = SDL_CreateRenderer(Window, 0, flags);
    if (Renderer == nullptr)
        return;

    CellTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, GridWidth, GridHeight);
    if (CellTexture == nullptr)
        return;

    //Clear the grid.
    memset(Cells, 0, sizeof(Cell) * GridArea);
 
    CreateCellSquare((WindowWidth / 2), (WindowHeight / 4), 200, WHITE);

    IsRunning = true;
}

void Destroy()
{
    SDL_DestroyTexture(CellTexture);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

int main()
{
    srand(time(NULL));

    Create();

    while (IsRunning)
    {
        Update();
        Render();
    }

    Destroy();

    return 0;
}