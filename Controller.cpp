#include "Controller.h"
#include "Engine.h"
#include <iostream>
#include <fstream>
#include <cmath>



Controller::Controller() {
	


	load_textures();
	


};



void Controller::load_textures() {
	





	for (int b = 0; b < 2; b++)
	{

		FILE* f;
		errno_t err = fopen_s(&f, b == 0 ? "sprites.bmp" : "font.bmp", "rb");

		int image_size = b == 0 ? SPRITES_IMAGE_SIZE : FONT_IMAGE_SIZE;


		unsigned char info[54];
		fread(info, sizeof(unsigned char), 54, f);

		int size = 3 * image_size * image_size;
		unsigned char* data = new unsigned char[size];
		fread(data, sizeof(unsigned char), size, f);



		int i = 0;
		for (int y = 0; y < image_size; y++)
			for (int x = 0; x < image_size; x++)
			{

				uint32_t col = (data[i]) + (data[i + 1] << 8) + (data[i + 2] << 16);

				if (b == 0)
					sprites_pixels[image_size - y - 1][x] = col;
				else
					font_pixels[image_size - y - 1][x] = col;


				i += 3;
			}

	}


	


	

}

void Controller::start() {

	state = MENU;
	click_cooldown = 0;

	clear_entities();
	place_rocks();






	score = 0;
	lives = 3;


	stars.clear();
	for (int i = 0; i < 20; i++)
		stars.push_back({ rand() % SCENE_WIDTH,rand() % SCENE_HEIGHT });


}


void Controller::clear_entities()
{
	entities.clear();
}

void Controller::place_rocks()
{

	


	for (int i = 0; i < 10; i += 1)
	{
		float aa = i * 36.0 / 180 * PI;

		scene_add_object((new Rock(
			this,
			SCENE_WIDTH / 2 + cos(aa) * (SCENE_WIDTH / 3.0),
			SCENE_HEIGHT / 2 + sin(aa) * (SCENE_HEIGHT / 3.0),
			rand() % 360,
			15 + rand() % 50,
			rand() % 3, rand() % 3)));


	}
}


void Controller::draw_sprite(float x, float y, float rotation, int sx, int sy, int sw, int sh,int transparency,bool is_text)
{

	int cx = x * GAME_SCALE;
	int cy = y * GAME_SCALE;

	for (int ix=0;ix<sw;ix++)
		for (int iy = 0; iy < sh; iy++)
			if ((ix + iy) % transparency == 0)
			{
			

				float dx = (ix - sw / 2) * GAME_SCALE;
				float dy = (iy - sh / 2) * GAME_SCALE;
				float dr = rotation * PI / 180;
			
				int px = cx + cos(dr) * dx - sin(dr) * dy;
				int py = cy + sin(dr) * dx + cos(dr) * dy;


				uint32_t col = is_text ? font_pixels[sy + iy][sx + ix] : sprites_pixels[sy + iy][sx + ix];

				if (col != 0x000000)
					for (int xx=0;xx<GAME_SCALE;xx+=1)
						for (int yy = 0; yy < GAME_SCALE; yy += 1)
						{


							int puty = py + yy - GAME_SCALE / 2;
							int putx = px + xx - GAME_SCALE / 2;


							while (putx < 0)
								putx += SCREEN_WIDTH;
							while (putx >= SCREEN_WIDTH)
								putx -= SCREEN_WIDTH;

							while (puty < 0)
								puty += SCREEN_HEIGHT;
							while (puty >= SCREEN_HEIGHT)
								puty -= SCREEN_HEIGHT;
					
							buffer[puty][putx] = col;
						}
						
				}
	

}

void Controller::draw_text(std::string s,float x, float y, int align_type)
{
	int i = 0;
	int w = 16 - 2;
	for (char& c : s) {
		draw_sprite(x + i*w - align_type*w*s.length()/2,y,0,(c % 16) *16, (c / 16) * 16,16,16, 1,true);
		i += 1;
	}

}


void Controller::draw() {


	for (int i = 0; i < stars.size(); i++)
		draw_sprite(stars[i].first, stars[i].second, 0, 48 + (i % 2 == 0 ? 0 : 2), 0, 2, 2, 1, false);
		


	for (auto e = entities.begin(); e != entities.end(); e++)
		(*e)->draw();

	switch (state)
	{
	case MENU:

		draw_text("ASTEROIDS", SCENE_WIDTH/2, 30,1);

		draw_text("PRESS SPACE", SCENE_WIDTH / 2, 190, 1);
		draw_text("TO START", SCENE_WIDTH / 2, 210, 1);
		
		break;
	case PLAYING:
		draw_text("LIVES: ", 20, 20,0);

		for (int i = 0; i < lives; i++)
			draw_sprite(105 + i * 16, 20, 0, 24, 0, 8, 16, 1, false);

		draw_text("SCORE: " + std::to_string(score), SCENE_WIDTH - 20, 20,2);
		break;
	case OVER_BAD:
		draw_text("GAME OVER. YOU LOSE", SCENE_WIDTH / 2, 40, 1);
		break;
	case OVER_GOOD:
		draw_text("GAME OVER. YOU WIN", SCENE_WIDTH / 2, 40, 1);

		draw_text("SCORE: " + std::to_string(score), SCENE_WIDTH / 2, 60, 1);
		break;
	}

	if ((state == OVER_BAD || state == OVER_GOOD) && click_cooldown <= 0 )
	{
		draw_text("PRESS SPACE", SCENE_WIDTH / 2, 190, 1);
		draw_text("TO RESTART", SCENE_WIDTH / 2, 210, 1);
	}

}

void Controller::update(float dt) {

	for (int i = 0; i < stars.size(); i++)
		stars[i].first -= (i < stars.size() / 2 ? 10 : 5)*dt;


	if (click_cooldown > 0)
		click_cooldown -= dt;
	else
		

		if (is_key_pressed(VK_SPACE))
		{
			if (state == MENU)
			{
				state = PLAYING;
				clear_entities();
				place_rocks();
				scene_add_object((new Player(this, SCENE_WIDTH / 2, SCENE_HEIGHT / 2)));

			}
			else if (state == OVER_GOOD || state == OVER_BAD)
			{
			
				start();
			}
		}
		
	
	if (entities.size() > 0)
	{

		auto update_entities = entities;


		int rocks = 0;

		for (auto e = update_entities.begin(); e != update_entities.end(); e++)
		{

			(*e)->update(dt);

			if ((*e)->collision_group == Entity::CollisionGroup::ROCK)
				rocks += 1;
		}




		update_entities = entities;


		for (int i = 0; i < update_entities.size() - 1; i++)
			for (int j = i + 1; j < update_entities.size(); j++)
				if (update_entities[i]->check_collision(update_entities[j]))
				{
					update_entities[i]->collide(update_entities[j]);
					update_entities[j]->collide(update_entities[i]);
				}



		if (rocks == 0 && state == PLAYING)
		{
			state = OVER_GOOD;

			click_cooldown = 2;
		}
	}




}

void Controller::scene_add_object(Entity* object) {
	entities.push_back(object);
}

void Controller::scene_remove_object(Entity* object) {
	
	//std::remove(entities.begin(), entities.end(), object);



	auto it = find(entities.begin(), entities.end(), object);
	if (it != entities.end())
		entities.erase(it);
}

void Controller::score_inc(int s) {
	score += s;

}
void Controller::player_die() {
	

	if (lives > 0)
	{
		lives -= 1;
		scene_add_object((new Player(this, SCENE_WIDTH / 2, SCENE_HEIGHT / 2)));
	}
	else
	{
		state = OVER_BAD;

		click_cooldown = 2;
		
	}
}