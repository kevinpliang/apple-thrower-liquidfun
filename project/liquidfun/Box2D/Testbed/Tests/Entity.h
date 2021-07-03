#ifndef ENTITY_H
#define ENTITY_H

#include <Box2D/Box2D.h>
#include <random>
#include "GL/freeglut.h"
#include "Apple.h"

class Entity {
public: 
	// body variables
	b2BodyDef entity;
	b2Body* body;
	b2PolygonShape shape;
	b2FixtureDef fixture;

    // type variables
    enum entity_type { player_type, enemy_type };
    entity_type my_type;

    // physics variables
    int max_health;
    int curr_health;
	int32 speed = 1.0f;
	b2Vec2 velocity = b2Vec2(0, 0);
	int32 numContacts;

    // constructor
	Entity(b2World* m_world) {
        // creates body, shape, fixture
		entity.type = b2_dynamicBody;
		entity.allowSleep = false;
		body = m_world->CreateBody(&entity);	
		shape.SetAsBox(0.1f, 0.1f, b2Vec2( 0.0f, 0.0f), 0.0);
		fixture.shape = &shape;
		fixture.density = 0.1f;
		fixture.friction = 0;
		body->CreateFixture(&fixture);

        // applies relevant transformations
		body->SetFixedRotation(true);
		body->SetUserData(this);
		numContacts = 0;
	}

	~Entity() {}	

    // sets entities velocity as vector
	void setVel(b2Vec2 v) {
		this->velocity = v;
		body->SetLinearVelocity(v);
	}

    // called by step every timestep
    void process() {
        renderAtBody();
        if (numContacts > 0) {
            --curr_health;
        }
    }

    // draws entity
	virtual void render() = 0;

    // moves entity-drawing to correct position
	void renderAtBody() {
		b2Vec2 pos = body->GetPosition();
		float angle = body->GetAngle();

		glPushMatrix();
		glTranslatef( pos.x, pos.y, 0);
		glRotatef( angle * (180/3.14), 0, 0, 1);
		render();
		glPopMatrix();
	}

    // called when contacting an object
	virtual void startContact(b2Body* contact) = 0;

    // called when no longer contacting an object
	virtual void endContact(b2Body* contact) = 0;

	double randomFloat(int a, int b) {
        srand((unsigned)time(NULL));
		float ok = (float) rand()/RAND_MAX;
        return (ok*(b-a)+a);
	}

};

#endif