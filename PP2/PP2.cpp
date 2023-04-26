#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480


// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
	SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};


#define SIZE 32

// initialize a game
void init(int map[99][99], bool mapTarget[99][99], int *mapEmptyX, int *mapEmptyY, int *mapBoxX, int *mapBoxY, int *mapTargetX, int *mapTargetY, int &mapPlayerX, int &mapPlayerY)
{
	for (int i = 0; i < 16; i++)
		map[mapEmptyX[i]][mapEmptyY[i]] = 3;
	for (int i = 0; i < 6; i++)
		map[mapBoxX[i]][mapBoxY[i]] = 2;
	map[3][6] = 2;
	mapTarget[3][6] = true;
	for (int i = 0; i < 6; i++) {
		map[mapTargetX[i]][mapTargetY[i]] = 3;
		mapTarget[mapTargetX[i]][mapTargetY[i]] = true;
	}
	mapPlayerX = 2;
	mapPlayerY = 2;
	map[mapPlayerX][mapPlayerY] = 1;
}

// draw a map
void drawMap(int map[99][99], bool mapTarget[99][99], SDL_Surface *charset, SDL_Surface *screen, SDL_Surface *sciana, SDL_Surface *czlowiek, SDL_Surface *czlowiekdoc, SDL_Surface *wolne, SDL_Surface *docelowe, SDL_Surface *skrzynia, SDL_Surface *skrzyniadoc, SDL_Renderer *renderer, SDL_Texture *scrtex, int czarny, int czerwony, int niebieski, double worldTime, double fps, char *text)
{
	SDL_FillRect(screen, NULL, czarny);
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 9; j++) {
			if (map[i][j] == 0)
				DrawSurface(screen, sciana, SCREEN_WIDTH / 2 - 4 * SIZE + i * SIZE, SCREEN_HEIGHT / 2 - 4 * SIZE + j * SIZE);
			else if (map[i][j] == 1) {
				if (!mapTarget[i][j])
					DrawSurface(screen, czlowiek, SCREEN_WIDTH / 2 - 4 * SIZE + i * SIZE, SCREEN_HEIGHT / 2 - 4 * SIZE + j * SIZE);
				else
					DrawSurface(screen, czlowiekdoc, SCREEN_WIDTH / 2 - 4 * SIZE + i * SIZE, SCREEN_HEIGHT / 2 - 4 * SIZE + j * SIZE);
			}
			else if (map[i][j] == 3) {
				if (!mapTarget[i][j])
					DrawSurface(screen, wolne, SCREEN_WIDTH / 2 - 4 * SIZE + i * SIZE, SCREEN_HEIGHT / 2 - 4 * SIZE + j * SIZE);
				else
					DrawSurface(screen, docelowe, SCREEN_WIDTH / 2 - 4 * SIZE + i * SIZE, SCREEN_HEIGHT / 2 - 4 * SIZE + j * SIZE);
			}
			else if (map[i][j] == 2) {
				if (!mapTarget[i][j])
					DrawSurface(screen, skrzynia, SCREEN_WIDTH / 2 - 4 * SIZE + i * SIZE, SCREEN_HEIGHT / 2 - 4 * SIZE + j * SIZE);
				else
					DrawSurface(screen, skrzyniadoc, SCREEN_WIDTH / 2 - 4 * SIZE + i * SIZE, SCREEN_HEIGHT / 2 - 4 * SIZE + j * SIZE);
			}

		}

	// tekst informacyjny / info text
	DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
	sprintf(text, "Sokoban, czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
	sprintf(text, "Esc - wyjscie, n - nowa gra, strzalki - poruszanie sie");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	//		SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}


// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	int t1, t2, quit, frames, rc, predkosc = 120;
	double delta, worldTime, fpsTimer, fps = 0;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *eti, *sciana, *wolne, *czlowiek, *skrzynia, *docelowe, *czlowiekdoc, *skrzyniadoc;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// tryb pe³noekranowy / fullscreen mode
	//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
	//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&window, &renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Patryk Pardej 165249");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);


	// disable mouse cursor visibility
	SDL_ShowCursor(SDL_DISABLE);

	// loading a cs8x8.bmp image
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	// loading imgs to be put on a map
	sciana = SDL_LoadBMP("./obrazy/sciana.bmp");
	wolne = SDL_LoadBMP("./obrazy/wolne.bmp");
	czlowiek = SDL_LoadBMP("./obrazy/czlowiek.bmp");
	czlowiekdoc = SDL_LoadBMP("./obrazy/czlowiekdoc.bmp");
	skrzynia = SDL_LoadBMP("./obrazy/skrzynia.bmp");
	skrzyniadoc = SDL_LoadBMP("./obrazy/skrzyniadoc.bmp");
	docelowe = SDL_LoadBMP("./obrazy/docelowe.bmp");

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;

	// array map takes values:
	// 0 - if on the place with given coords there is a wall
	// 1 - if there is a player
	// 2 - if there's a box
	// 3 - if there's an empty field that the player can walk into
	int map[99][99];
	for (int i = 0; i < 99; i++)
		for (int j = 0; j < 99; j++)
			map[i][j] = 0;

	// array mapTarget takes values:
	// true - if the field is with a pink dot, meaning it is a field where the box has to be put
	// false - otherwise
	bool mapTarget[99][99];
	for (int i = 0; i < 99; i++)
		for (int j = 0; j < 99; j++)
			mapTarget[i][j] = false;

	//creating a default map 8x9 (coordinates of elements on the map)

	//coords of empty fields { (1,5), (1,7), (2,6), ... }
	int mapEmptyX[16] = { 1,1,2,2,3,3,3,3,4,4,5,5,5,5,5,6 };
	int mapEmptyY[16] = { 5,7,6,7,1,3,5,7,1,2,1,2,4,5,7,7 };
	//coords of fields with boxes
	int mapBoxX[6] = { 1,3,4,4,4,5 };
	int mapBoxY[6] = { 6,2,3,4,6,6 };
	//coords of fields with target fields (with pink dots)
	int mapTargetX[6] = { 1,1,4,4,5,6 };
	int mapTargetY[6] = { 2,4,5,7,3,6 };
	//coords of a field with a player on a starting position
	int mapPlayerX = 2;
	int mapPlayerY = 2;
	//initialize with default coords
	init(map, mapTarget, mapEmptyX, mapEmptyY, mapBoxX, mapBoxY, mapTargetX, mapTargetY, mapPlayerX, mapPlayerY);

	//play
	while (!quit) {
		t2 = SDL_GetTicks();

		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplyna³ od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		drawMap(map, mapTarget, charset, screen, sciana, czlowiek, czlowiekdoc, wolne, docelowe, skrzynia, skrzyniadoc, renderer, scrtex, czarny, czerwony, niebieski, worldTime, fps, text);

		fpsTimer += delta;
		if (fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};

		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				// up arrow - go up if there is no wall/box
				if (event.key.keysym.sym == SDLK_UP) {
					if (map[mapPlayerX][mapPlayerY - 1] == 3) {
						map[mapPlayerX][mapPlayerY] = 3;
						mapPlayerY = mapPlayerY - 1;
						map[mapPlayerX][mapPlayerY] = 1;
					}
					else if (map[mapPlayerX][mapPlayerY - 1] == 2 && map[mapPlayerX][mapPlayerY - 2] == 3) {
						map[mapPlayerX][mapPlayerY] = 3;
						mapPlayerY = mapPlayerY - 1;
						map[mapPlayerX][mapPlayerY] = 1;
						map[mapPlayerX][mapPlayerY - 1] = 2;
					}
				}
				// down arrow - go down if there is no wall/box
				else if (event.key.keysym.sym == SDLK_DOWN) {
					if (map[mapPlayerX][mapPlayerY + 1] == 3) {
						map[mapPlayerX][mapPlayerY] = 3;
						mapPlayerY = mapPlayerY + 1;
						map[mapPlayerX][mapPlayerY] = 1;
					}
					else if (map[mapPlayerX][mapPlayerY + 1] == 2 && map[mapPlayerX][mapPlayerY + 2] == 3) {
						map[mapPlayerX][mapPlayerY] = 3;
						mapPlayerY = mapPlayerY + 1;
						map[mapPlayerX][mapPlayerY] = 1;
						map[mapPlayerX][mapPlayerY + 1] = 2;
					}
				}
				// right arrow - go right if there is no wall/box
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					if (map[mapPlayerX + 1][mapPlayerY] == 3) {
						map[mapPlayerX][mapPlayerY] = 3;
						mapPlayerX = mapPlayerX + 1;
						map[mapPlayerX][mapPlayerY] = 1;
					}
					else if (map[mapPlayerX + 1][mapPlayerY] == 2 && map[mapPlayerX + 2][mapPlayerY] == 3) {
						map[mapPlayerX][mapPlayerY] = 3;
						mapPlayerX = mapPlayerX + 1;
						map[mapPlayerX][mapPlayerY] = 1;
						map[mapPlayerX + 1][mapPlayerY] = 2;
					}
				}
				// left arrow - go left if there is no wall/box
				else if (event.key.keysym.sym == SDLK_LEFT) {
					if (map[mapPlayerX - 1][mapPlayerY] == 3) {
						map[mapPlayerX][mapPlayerY] = 3;
						mapPlayerX = mapPlayerX - 1;
						map[mapPlayerX][mapPlayerY] = 1;
					}
					else if (map[mapPlayerX - 1][mapPlayerY] == 2 && map[mapPlayerX - 2][mapPlayerY] == 3) {
						map[mapPlayerX][mapPlayerY] = 3;
						mapPlayerX = mapPlayerX - 1;
						map[mapPlayerX][mapPlayerY] = 1;
						map[mapPlayerX - 1][mapPlayerY] = 2;
					}
				}
				// escape - quit
				else if (event.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				// 'n' - new game
				else if (event.key.keysym.sym == SDLK_n) {
					init(map, mapTarget, mapEmptyX, mapEmptyY, mapBoxX, mapBoxY, mapTargetX, mapTargetY, mapPlayerX, mapPlayerY);
					worldTime = 0;
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
		frames++;
	};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
};
