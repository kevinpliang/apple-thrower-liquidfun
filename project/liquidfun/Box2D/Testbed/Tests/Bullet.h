#ifndef BULLET_H
#define BULLET_H

#include <Box2D/Box2D.h>
#include <random>
#include "GL/freeglut.h"
#include "Apple.h"

class Bullet {
public: 
	// body variables
	b2BodyDef bullet;
	b2Body* bulletBody;
	b2CircleShape bulletShape;
	b2FixtureDef bulletFixture;

    // physics variables
	int32 speed = 5.0f;
	b2Vec2 velocity = b2Vec2(0, 0);
	int32 numContacts;

    // constructor
	Bullet(b2World* m_world) {
        // creates body, shape, fixture
		bullet.type = b2_dynamicBody;
		bullet.allowSleep = false;
		bulletBody = m_world->CreateBody(&bullet);
		bulletShape.m_radius = 0.05f;
		bulletFixture.shape = &bulletShape;
		bulletFixture.density = 0.1f;
		bulletFixture.friction = 0;
		bulletBody->CreateFixture(&bulletFixture);

        // applies relevant transformations
		bulletBody->SetFixedRotation(true);
		numContacts = 0;
	}

	~Bullet() {}	

    // sets bullets position
    void setPos(b2Vec2 p) {
        bulletBody->SetTransform(p,bulletBody->GetAngle());
    }

    // sets bullets velocity as vector
	void setVel(b2Vec2 v) {
		this->velocity = v;
		bulletBody->SetLinearVelocity(v);
	}

    // draws entity
	void render() {
        const float32 k_segments = 16.0f;
        const float32 k_increment = 2.0f * b2_pi / k_segments;
        float32 theta = 0.0f;
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        for (int32 i = 0; i < k_segments; ++i) {
            b2Vec2 v = bulletBody->GetPosition() + bulletShape.m_radius * b2Vec2(cosf(theta), sinf(theta));
            glVertex2f(v.x, v.y);
            theta += k_increment;
        }
        glEnd();
    }

    // moves entity-drawing to correct position
	void renderAtBody() {
		b2Vec2 pos = bulletBody->GetPosition();
		float angle = bulletBody->GetAngle();

		glPushMatrix();
		glTranslatef( pos.x, pos.y, 0);
		glRotatef( angle * (180/3.14), 0, 0, 1);
		render();
		glPopMatrix();
	}

    // called when contacting an object
	void startContact() {
		++numContacts;
	}

    // called when no longer contacting an object
	void endContact() {
		--numContacts;
	}

};

#endif