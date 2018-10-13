#include <SDL2/SDL.h>
#include <SDL2/SDL_Timer.h>
#include <SDL2/SDL_Log.h>
#include <SDL2/SDL_Mouse.h>
#include <algorithm>
#include <vector>
#include "curvylines.h"

#define Logme(x, ...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, x, ##__VA_ARGS__)


// Spline stuff
Spline::Spline() {
    r = rand() % 255;
    g = rand() % 255;
    b = rand() % 255;
}

void Spline::setRGB(int r_, int g_, int b_) {
    r = r_;
    g = g_;
    b = b_;
}


void Spline::getStart(int& x1_, int& y1_) {
    x1_ = x1;
    y1_ = y1;
}

void Spline::getEnd(int& x2_, int& y2_) {
    x2_ = x2;
    y2_ = y2;
}

void Spline::SetStart(int x, int y) {
    x1 = x;
    y1 = y;
}

void Spline::SetEnd(int x, int y) {
    x2 = x;
    y2 = y;
}

bool Spline::HasStart() const {
    if (x1 > -1 && y1 > -1) {
        return true;
    }
    return false;
}

bool Spline::HasEnd() const {
    if (x2 > -1 && y2 > -1) {
        return true;
    }
    return false;
}

void Spline::Render(SDL_Renderer **ren) const {
    if (HasStart() && HasEnd()) {
        SDL_SetRenderDrawColor(*ren, r, g , b, 255);
        SDL_RenderDrawLine(*ren, x1, y1, x2, y2);
    }
}


void cleanup(SDL_Window **win) {
	Logme("BOOM");
	SDL_DestroyWindow(*win);
	SDL_Quit();
}

bool init(SDL_Window **win, SDL_Renderer **ren) {

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }

    if (getenv("LOG")) {
        SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    }

    Logme("Everything Initiated");

    if (SDL_CreateWindowAndRenderer(512, 512, SDL_WINDOW_RESIZABLE, win, ren) !=0) {
		return false;
	}

    Logme("Window and Renderer Created");

    return true;
}


// BEGIN AT THE BEGINNING
int main(int argc, char const *argv[])
{
    const size_t interpCount = 100;

    // Basic SDL stuff
    SDL_Window *window;
    SDL_Renderer *renderer;
    init(&window, &renderer);

    // We need two splines
    //std::vector<Spline> splines(2);
    std::vector<Spline> drawObjects;
    //std::size_t splount = 0;

    bool down = false;

    while (1) {
        SDL_Event e;
        SDL_WaitEventTimeout(&e, 500);

        if (e.type == SDL_QUIT) {
            cleanup(&window);
            return 0;
        }

        // on mousedown set startpoint for the correct spline
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            down = true;

            if (drawObjects.size() >= 2) {
                drawObjects.clear();
                Logme("Zero, bitches!");
            }

            Logme("DOWN! tx=%d, y=%d", e.button.x, e.button.y);

            Spline spliny;
            spliny.SetStart(e.button.x, e.button.y);
            spliny.SetEnd(e.button.x, e.button.y);
            drawObjects.push_back(spliny);
        }

        if (e.type == SDL_MOUSEMOTION && down) {
            int x, y;
            SDL_GetMouseState( &x, &y );

            drawObjects.back().SetEnd(x, y);

            Logme("MOVING! x=%d, y=%d", x, y);
        }

        // on mouseup set endpoint for the correct spline
        if (e.type == SDL_MOUSEBUTTONUP) {
            down = false;

            Logme("UP! tx=%d, y=%d", e.button.x, e.button.y);

            drawObjects.back().SetEnd(e.button.x, e.button.y);

            if (drawObjects.size() ==2) { // we have two Splines, interpolate!
                int x1a, y1a, x2a, y2a;
                drawObjects[0].getStart(x1a, y1a);
                drawObjects[0].getEnd(x2a, y2a);

                int x1b, y1b, x2b, y2b;
                drawObjects[1].getStart(x1b, y1b);
                drawObjects[1].getEnd(x2b, y2b);

                double slopea = (y2a - y1a) / static_cast<double>(x2a - x1a);
                double slopeb = (y2b - y1b) / static_cast<double>(x2b - x1b);
                
                double deltaa = (x2a - x1a) / static_cast<double>(interpCount);
                double deltab = (x2b - x1b) / static_cast<double>(interpCount);

                Logme("%g,%g, %g, %g", slopea, slopeb, deltaa, deltab);


                for (size_t i =0; i <= interpCount; ++i) {
                    Spline foo;
                    foo.SetStart( static_cast<int>(x1a + i * deltaa), static_cast<int>(y1a + i * deltaa * slopea));
                    foo.SetEnd(static_cast<int>(x1b + i * deltab), static_cast<int>(y1b + i * deltab * slopeb));
                    foo.setRGB(255.0 / interpCount * i, 155, 155);
                    drawObjects.push_back(foo);

                    Logme("%d, %d, %d, %d", static_cast<int>(x1a + i * deltaa), static_cast<int>(y1a + i * deltaa * slopea), 
                        static_cast<int>(x1b + i * deltab), static_cast<int>(y1b + i * deltab * slopeb));
                }

            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0 , 0, 255);
        SDL_RenderClear(renderer);

        // on mouseup set endpoint for the right spline
        // render the splines and curvy lines
        std::for_each(drawObjects.begin(), drawObjects.end(), [&renderer](const Spline &spline_) { spline_.Render(&renderer); });

        SDL_RenderPresent(renderer);
    }
	
    return 0;
}
