#pragma once

class Controller;

class Entity {

	


public:
	enum CollisionGroup { NONE, PLAYER, BULLET, ROCK };



	Entity(Controller* controller, CollisionGroup collision_group, float radius);


	void draw();
	
	
	
	virtual void update(float dt);
	virtual void collide(Entity* other);


	bool check_collision(Entity* other);
	void destroy();

	CollisionGroup collision_group;
public:
	float x;
	float y;
protected:
		
		
	Controller* controller;


	int sprite_x;
	int sprite_y;
	int sprite_width;
	int sprite_height;

	
	float rotation;
	int transparency;

	float speed_x;
	float speed_y;

	float max_speed;
	float deceleration;


	bool loop_borders;
	
	float radius;

	



};



class Player : public Entity {
	
public:
	Player(Controller* controller,float x, float y);

	void update(float dt);
	void collide(Entity* other);

protected:

	float reloaded;
	float immortal_cooldown;

};

class Rock : public Entity {
public:
	Rock(Controller* controller, float x, float y, float direction, float speed, int size,int type);

	void update(float dt);
	void collide(Entity* other);

protected:

	float direction;
	int size;
	int type;

};

class Bullet : public Entity {
public:
	Bullet(Controller* controller, float x, float y, float direction, float speed);
	void update(float dt);
	void collide(Entity* other);
};

class Particle : public Entity {
public:
	Particle(Controller* controller, float x, float y, float direction, float speed, int sprite_x, int sprite_y, int sprite_width, int sprite_height);

	void update(float dt);
	void collide(Entity* other);


protected:
	float tr_step;
};




