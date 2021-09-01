#include "Entity.h"
#include "Controller.h"
#include "Engine.h"

Entity::Entity(Controller* controller, CollisionGroup collision_group, float radius) {
	this->controller = controller;

	this->collision_group = collision_group;

	this->radius = radius;


	
}


void Entity::update(float dt) {

	speed_x -= speed_x * deceleration * dt;
	speed_y -= speed_y * deceleration * dt;


	if (max_speed != -1)
	{
		float m_spd = sqrt(pow(speed_x, 2) + pow(speed_y, 2));

		if (m_spd > max_speed)
		{
			speed_x = speed_x / m_spd * max_speed;
			speed_y = speed_y / m_spd * max_speed;
		}
			
	
	}
	
	
	

	x += speed_x * dt;
	y += speed_y * dt;


	if (loop_borders)
	{

		while (x < 0)
			x += SCENE_WIDTH;
		while (x > SCENE_WIDTH)
			x -= SCENE_WIDTH;

		while (y < 0)
			y += SCENE_HEIGHT;
		while (y > SCENE_HEIGHT)
			y -= SCENE_HEIGHT;
	}
	else if (x < 0 || y<0 || x > SCENE_WIDTH || y > SCENE_HEIGHT)
		destroy();


}

bool Entity::check_collision(Entity* other) {


	return sqrt(pow(other->x - x, 2) + pow(other->y - y, 2)) < (radius + other->radius);
}

void Entity::collide(Entity* other)
{
	

	return;
}

void Entity::draw() {
	controller->draw_sprite(x, y, rotation, sprite_x, sprite_y, sprite_width, sprite_height, transparency, false);


}
void Entity::destroy() {
	controller->scene_remove_object(this);


}


Player::Player(Controller* controller, float x,float y) : Entity(controller,PLAYER,7) {



	this->x = x;
	this->y = y;


	sprite_x = 0;
	sprite_y = 0;

	sprite_width = 16;
	sprite_height =16;

	max_speed = 140;
	deceleration = 0.25;

	
	loop_borders = true;
	transparency = 1;


	reloaded = 0.5;

	immortal_cooldown = 1;

}


void Player::update(float dt)
{


	if (immortal_cooldown > 0)
	{
		immortal_cooldown -= dt;
		transparency = 3 + (int)(2.0 * sin(immortal_cooldown*30.0));
	}
	else
		transparency = 1;
		

	float acc = 190 * dt;

	/*
	if (is_key_pressed(VK_LEFT))
		speed_x -= acc;
	if (is_key_pressed(VK_RIGHT))
		speed_x += acc;
	if (is_key_pressed(VK_UP))
		speed_y -= acc;
	if (is_key_pressed(VK_DOWN))
		speed_y += acc;*/

	if (is_key_pressed(VK_LEFT))
		rotation -= acc;

	if (is_key_pressed(VK_RIGHT))
		rotation += acc;

	if (is_key_pressed(VK_UP))
	{
		speed_x += cos(rotation / 180 * PI) * acc;
		speed_y += sin(rotation / 180 * PI) * acc;
	}

	if (is_key_pressed(VK_DOWN))
	{
		speed_x -= cos(rotation / 180 * PI) * acc/4;
		speed_y -= sin(rotation / 180 * PI) * acc/4;
	}
		

	if (reloaded > 0)
		reloaded -= dt;
	else

		if (is_key_pressed(VK_SPACE))
		{
			controller->scene_add_object(new Bullet(controller, x, y, rotation, 250));
			reloaded = 0.25;
		}

	Entity::update(dt);


	//rotation = atan2(get_cursor_y() - y*GAME_SCALE, get_cursor_x()-x * GAME_SCALE) / PI * 180;

		



}

void Player::collide(Entity* other) {
	switch (other->collision_group)
	{
	case ROCK:

		if (immortal_cooldown <= 0)
		{

			controller->player_die();


			destroy();
			for (int i = 0; i < 3; i++)
				controller->scene_add_object(new Particle(
					controller, x, y,
					rand() % 360,
					15 + rand() % 5,
					(rand() % 8),
					(rand() % 8),
					8, 8));

			break;
		}

	}



}


Rock::Rock(Controller* controller, float x, float y, float direction, float speed, int size, int type) : Entity(controller,ROCK,4+size*2) {



	this->x = x;
	this->y = y;
	
	this->size = size;
	this->type = type;


	sprite_x = size*16;
	sprite_y = 16+type*16;

	sprite_width = 8 + 4 * size;
	sprite_height = 8 + 4 * size;

	transparency = 1;


	this->direction = direction;

	//speed_x = (-1+2*(rand() % 2))*(15+rand() % 50);
	//speed_y = (-1+2*(rand() % 2))*(15+rand() % 50);
	speed_x += cos(direction / 180 * PI) * speed;
	speed_y += sin(direction / 180 * PI) * speed;




	max_speed = -1;
	deceleration = 0;

	loop_borders = true;

}


void Rock::update(float dt)
{


	rotation +=( speed_x + speed_y) * dt;

	Entity::update(dt);
}


void Rock::collide(Entity* other) {

	



	switch (other->collision_group)
	{
	case BULLET:
		destroy();


		controller->score_inc(10 * (int)(sqrt(pow(x-other->x, 2) + pow(y-other->y, 2))));

		if (size > 0)
		{
			int sizes[] = { size - 1,0 };

			for (int i = 0; i < 2; i++)
				controller->scene_add_object(new Rock(controller, x, y,direction-45+(rand() % 90), 15 + rand() % 50, sizes[i], type));
		}
		else for (int i = 0; i < 3; i++)
			controller->scene_add_object(new Particle(
				controller, x, y, 
				rand() % 360, 
				5 + rand() % 5, 
				32 + (rand() % 12), 
				16 + 16 * type + (rand() % 12),
				4, 4));


		break;
	}



}



Bullet::Bullet(Controller* controller, float x, float y, float direction, float speed) : Entity(controller,BULLET,3) {



	this->x = x;
	this->y = y;


	sprite_x = 16;
	sprite_y = 0;

	sprite_width = 8;
	sprite_height = 8;

	rotation = direction;



	speed_x = cos(direction/180*PI)*speed ;
	speed_y = sin(direction/180*PI)*speed;


	max_speed = -1;
	deceleration = 0;

	loop_borders = false;
	transparency = 1;

}


void Bullet::update(float dt)
{



	Entity::update(dt);
}


void Bullet::collide(Entity* other) {
	switch (other->collision_group)
	{
	case ROCK:
		destroy();
		break;
	}



}








Particle::Particle(Controller* controller, float x, float y, float direction, float speed, int sprite_x, int sprite_y, int sprite_width, int sprite_height) : Entity(controller, NONE, 1) {



	this->x = x;
	this->y = y;


	this->sprite_x = sprite_x;
	this->sprite_y = sprite_y;

	this->sprite_width = sprite_width;
	this->sprite_height = sprite_height;

	rotation = direction;

	speed_x = cos(direction / 180 * PI) * speed;
	speed_y = sin(direction / 180 * PI) * speed;

	max_speed = -1;
	deceleration = 0;

	loop_borders = false;
	transparency = 2;

	tr_step = 1;
}



void Particle::update(float dt)
{

	rotation += (speed_x + speed_y) * dt;
	tr_step += dt;
	transparency = round(tr_step);

	Entity::update(dt);

}


void Particle::collide(Entity* other) {
	return;
}

