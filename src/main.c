/*** includes ***/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/*** defines ***/

#define WINDOW_TITLE "Simple Tic-Tac-Toe"
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 480

#ifdef _WIN64

#define SPRITES_SRC "res\\images\\"

#else

#define SPRITES_SRC "res/images/"

#endif

/*** data ***/

static int table[3][3] = {

	{ -1, -1, -1 },
	{ -1, -1, -1 },
	{ -1, -1, -1 }

};

int winner = -1;
int x, y, angle;

int player = 1;
int ai = 0;

int counter = 0;

SDL_Color ai_color = { 255, 35, 35, 255 };

SDL_Color player_color = { 65, 65, 255, 255 };

SDL_Color background_color = { 253, 255, 201, 255 };

void SwapShapes() {

	int t = player;
	int i, j;
	
	player = ai;
	ai = t;
	
	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			if (table[i][j] >= 0) table[i][j] = !table[i][j];
}

void SwapColors() {

	SDL_Color t = player_color;
	player_color = ai_color;
	ai_color = t;
	
}

void SetRenderColor(SDL_Renderer *renderer, SDL_Color c) {

	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

}

void DrawLine(int x, 
			  int y, 
			  int length, 
			  int size, 
			  int angle, 
			  SDL_Renderer *renderer){

	SDL_Texture *texture = SDL_CreateTexture(
		renderer, 
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_TARGET, 
		480, 
		480
	);
	
	SDL_Point point = { x, y };
	
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	
	SDL_Rect line = {x - length / 2, y - size / 2, length, size};

	SDL_SetRenderTarget(renderer, texture);
	
	SDL_RenderFillRect(renderer, &line);
	
	SDL_SetRenderTarget(renderer, NULL);
	
	SDL_RenderCopyEx(renderer, 
	                 texture, 
	                 NULL, 
	                 NULL, 
	                 angle, 
	                 &point, 
	                 SDL_FLIP_NONE);
	
	SDL_DestroyTexture(texture);
	
}

void DrawCross(int x, 
               int y, 
               int size, 
               int thickness, 
               SDL_Renderer *renderer) {
	
	float diag = size * sqrt(2);
	
	DrawLine(x, y, diag, thickness, 45, renderer);
	DrawLine(x, y, diag, thickness, -45, renderer);
	
}

void DrawCircle(int center_x, int center_y, int radius, SDL_Renderer *renderer) {

	int diameter = radius * 2;
	
	int x = (radius - 1);
	int y = 0;
	int tx = 1;
	int ty = 1;
	int error = (tx - diameter);
	
	while (x >= y) {
		
		SDL_RenderDrawLine(renderer, 
		                   center_x + x, 
		                   center_y - y, 
		                   center_x - x, 
		                   center_y - y);
		                   
		SDL_RenderDrawLine(renderer, 
		                   center_x + x, 
		                   center_y + y, 
		                   center_x - x, 
		                   center_y + y);
		                   
		SDL_RenderDrawLine(renderer, 
		                   center_x + y, 
		                   center_y - x, 
		                   center_x - y, 
		                   center_y - x);
		                   
		SDL_RenderDrawLine(renderer, 
		                   center_x + y, 
		                   center_y + x, 
		                   center_x - y, 
		                   center_y + x);
		
		if (error <= 0) {
			
			++y;
			error += ty;
			ty += 2;
			
		}
		
		if (error > 0) {
			
			--x;
			tx += 2;
			error += (tx - diameter);
		
		}
		
	}
	
}

void DrawThickCircle(int x, int y, int radius, SDL_Renderer *renderer) {

	DrawCircle(x, y, radius, renderer);
	SetRenderColor(renderer, background_color);
	DrawCircle(x, y, radius - 10, renderer);

}

void DrawPlayer(int x, int y, SDL_Renderer *renderer) {
	
	SetRenderColor(renderer, player_color);
	
	if(player == 0)
		DrawThickCircle(x, y, 50, renderer);
	else if(player == 1)
		DrawCross(x, y, 70, 10, renderer);

}

void DrawAI(int x, int y, SDL_Renderer *renderer) {

	SetRenderColor(renderer, ai_color);

	if(ai == 0)
		DrawThickCircle(x, y, 50, renderer);
	else if(ai == 1)
		DrawCross(x, y, 70, 10, renderer);

}

void MakeAIMove(int v){
	
	struct table_cell {
		int i;
		int j;
		struct table_cell *next;
	};
	
	int i;
	int j;
	int c = 0;
	
	
	struct table_cell *cells = malloc(sizeof(struct table_cell));
	struct table_cell *first_cell = cells;
	
	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			if (table[i][j] < 0) {

				cells->i = i;
				cells->j = j;
				cells->next = malloc(sizeof(struct table_cell));
				cells = cells->next;
				cells->next = NULL;
				c++;

			}

	if (c) {

		c = rand() % c;
		while(c){
			first_cell = first_cell->next;
			c--;
		}

		table[first_cell->i][first_cell->j] = v;

	}
	
}

int CheckWinner(int *x, int *y, int *angle) {
	
	int i;
	int j;
	
	for(i = 0; i < 3; i++)
		if ((table[i][0] == table[i][1]) && 
		    (table[i][1] == table[i][2])) {

			*x = 120 * (i + 1);
			*y = 240;
			*angle = 90;
			return table[i][0];

		};
		
	for(j = 0; j < 3; j++)
		if ((table[0][j] == table[1][j]) && 
		    (table[1][j] == table[2][j])) {

			*y = 120 * (j + 1);
			*x = 240;
			*angle = 0;
			return table[0][j];

		};
		
	if((table[0][0] == table[1][1]) && 
	   (table[1][1] == table[2][2])) {

		*x = 240;
		*y = 240;
		*angle = 45;
		return table[0][0];

	}
	
	if((table[0][2] == table[1][1]) && 
	   (table[1][1] == table[2][0])) {
		
		*x = 240;
		*y = 240;
		*angle = -45;
		return table[0][2];
	}
	
	return -1;

}

int main(int argc, char *argv[]) {
	
	/*** initialization ***/
	
	srand(time(NULL));
	
	if (SDL_Init(SDL_INIT_VIDEO |  SDL_INIT_TIMER )) {
		
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		
		return 1;
		
	};
	
	SDL_Window *window;
	
	window = SDL_CreateWindow(
		WINDOW_TITLE,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE
	);
	
	SDL_SetWindowResizable(window, 0);
	
	if (window == NULL) {
		
		SDL_Log("Unable to create window: %s", SDL_GetError());
		
		return 1;
		
	}
	
	SDL_Surface *surface; 
	
	surface = IMG_Load(SPRITES_SRC "icon.png");
 
	SDL_SetWindowIcon(window, surface);
	
	SDL_FreeSurface(surface);
	
	SDL_Renderer *renderer;
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	if (renderer == NULL) {
		
		SDL_Log("Unable to create renderer: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		
		return 1;
		
	}
	
	/*** main loop ***/
	
	SDL_Event event;
	
	int close_signal = 0;
	
	SDL_Rect vertical_line1 = {175, 80, 10, 320};
	
	SDL_Rect vertical_line2 = {295, 80, 10, 320};
	
	SDL_Rect horizontal_line1 = {80, 175, 320, 10};
	
	SDL_Rect horizontal_line2 = {80, 295, 320, 10};
	
	SDL_Rect square_color = {420, 20, 40, 40};
	
	while (!close_signal) {
		
		SetRenderColor(renderer, background_color);
		
		SDL_RenderClear(renderer);
		
		SDL_SetRenderDrawColor(renderer, 204, 121, 12, 255);
		
		DrawLine(108, 180, 135, 10, 0, renderer);
		DrawLine(240, 180, 135, 10, 0, renderer);
		DrawLine(108, 300, 135, 10, 0, renderer);
		DrawLine(178, 108, 135, 10, 90, renderer);
		DrawLine(178, 240, 135, 10, 90, renderer);
		DrawLine(178, 372, 135, 10, 90, renderer);

		DrawLine(372, 180, 135, 10, 0, renderer);
		DrawLine(240, 300, 135, 10, 0, renderer);
		DrawLine(372, 300, 135, 10, 0, renderer);
		DrawLine(302, 108, 135, 10, 90, renderer);
		DrawLine(302, 240, 135, 10, 90, renderer);
		DrawLine(302, 372, 135, 10, 90, renderer);
		
		SetRenderColor(renderer, player_color);
		
		SDL_RenderFillRect(renderer, &square_color);
		
		if(player)
			DrawCross(40, 40, 20, 10, renderer);
		else
			DrawThickCircle(40, 40, 20, renderer);
		
		int i, j;
		
		for(i = 0; i < 3; i++)
			for(j = 0; j < 3; j++)
				if(table[i][j] == ai)
					DrawAI(120 * (i + 1), 120 * (j + 1), renderer);
				else if(table[i][j] == player)
					DrawPlayer(120 * (i + 1), 120 * (j + 1), renderer);
		
		SDL_SetRenderDrawColor(renderer, 60, 60, 255, 255);
		
		if(winner == ai)
			SetRenderColor(renderer, ai_color);
			
		if(winner == player)
			SetRenderColor(renderer, player_color);
		
		if(winner >= 0)
			DrawLine(x, y, 300, 10, angle, renderer);
			
		
		SDL_SetRenderDrawColor(renderer, 60, 60, 255, 255);
		
		while (SDL_PollEvent(&event)) {
			
			switch (event.type) {
				
				case SDL_QUIT:
					close_signal = 1;
					break;
					
				case SDL_MOUSEBUTTONDOWN: 
					if ((winner < 0) && counter < 10) {
			
						int mx, my;
			
						SDL_GetMouseState(&mx, &my);
			
						for(i = 0; i < 3; i++)
							for(j = 0; j < 3; j++) {
								
								int check_mx1 = ((i + 1) * 120 - 60) < 
								                mx;
								int check_mx2 = ((i + 1) * 120 + 60) > 
								                mx;
								int check_my1 = ((j + 1) * 120 - 60) < 
								                my;
								int check_my2 = ((j + 1) * 120 + 60) > 
								                my;
								
								int check_mx = check_mx1 && check_mx2;
								int check_my = check_my1 && check_my2;
								
								if (check_mx && 
								    check_my && 
								    table[i][j] < 0) {
								
									table[i][j] = player;
									winner = CheckWinner(&x, 
									                     &y, 
									                     &angle);
								
									if (winner < 0) 
										MakeAIMove(ai);
								
									winner = CheckWinner(&x, 
									                     &y, 
									                     &angle);
									counter += 2;
			
								}
			
							}
						
						int check_swap = (mx > 20 && mx < 60) && 
						                 (my > 20 && my < 60);
						
						int check_color = (mx > 420 && mx < 460) && 
						                  (my > 20 && my < 60);
						
						if (check_swap)
							SwapShapes();
							
						if(check_color)
							SwapColors();
						 
					} else {
			
						for(i = 0; i < 3; i++)
							for(j = 0; j < 3; j++)
								table[i][j] = -1;
			
						winner = -1;
						counter = 0;
			
					}	
					
					break;
			}
		}
		
		SDL_RenderPresent(renderer);
		
		SDL_Delay(1000 / 60);
		
	}
	
	/*** freeing memory and quit ***/
	
	SDL_DestroyRenderer(renderer);
	
	SDL_DestroyWindow(window);
	
	SDL_Quit();
	
	return 0;
}

#ifdef _WIN64

int WinMain(int argc, char *argv[]){
	
	main(argc, argv);
	return 0;

}

#endif
