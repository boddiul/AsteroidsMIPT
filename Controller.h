#pragma once

#define FONT_IMAGE_SIZE 256
#define SPRITES_IMAGE_SIZE 64
#define GAME_SCALE 3

#define SCENE_WIDTH SCREEN_WIDTH/GAME_SCALE
#define SCENE_HEIGHT SCREEN_HEIGHT/GAME_SCALE




#define PI 3.14159265 



#include "Entity.h"



#include <stdint.h>
#include <string>
#include <vector>




class Controller {


	uint32_t font_pixels[FONT_IMAGE_SIZE][FONT_IMAGE_SIZE] = { 0 };
	uint32_t sprites_pixels[SPRITES_IMAGE_SIZE][SPRITES_IMAGE_SIZE] = { 0 };

public:

	enum GameState { MENU,PLAYING,OVER_GOOD,OVER_BAD };

	Controller();

	void start();

	void draw_sprite(float x, float y, float rotation, int sx, int sy, int sw, int sh,int transparency, bool is_text);
	void draw_text(std::string s, float x, float y, int align_type);

	void load_textures();


	void update(float dt);

	void draw();



	void clear_entities();

	void place_rocks();

	void scene_add_object(Entity* object);
	void scene_remove_object(Entity* object);

	void score_inc(int s);
	void player_die();
protected:
	std::vector<Entity*> entities;
	std::vector<std::pair<float, float>> stars;



	GameState state;
	int score;
	int lives;

	float click_cooldown;

};