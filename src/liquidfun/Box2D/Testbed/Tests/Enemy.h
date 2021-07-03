#ifndef ENEMY_H
#define ENEMY_H

#include <Box2D/Box2D.h>
#include <random>
#include <time.h>
#include "GL/freeglut.h"
#include "Apple.h"
#include "Entity.h"

class Enemy : public Entity {
public: 
	Enemy(b2World* m_world, double h) : Entity(m_world) {
        my_type = enemy_type;
        max_health = h;
        curr_health = max_health;
        body->SetTransform(b2Vec2(randomFloat(-2, 2), 0.0f),body->GetAngle());
	}

	~Enemy() {}	

    // moves enemy towards player
    void move(b2Vec2 playerPos) {
        b2Vec2 moveVector = playerPos - body->GetPosition();
        moveVector.Normalize();
        setVel(speed * moveVector);
    }

	void render() {
		if (numContacts > 0) {
			glColor3f(1, 0, 0); // red
		} else {
			glColor3f(1,1,0);   // yellow
		}

		// nose and eyes
		glPointSize(4);
		glBegin(GL_POINTS);
		glVertex2f( 0, 0 );
		glVertex2f(-0.05, 0.05 );
		glVertex2f( 0.05, 0.05 );
		glEnd();
		
        // eyebrows
        glBegin(GL_LINES);
        glVertex2f(-0.05, 0.08);
        glVertex2f(0, 0.05);
        glVertex2f(0, 0.05);
        glVertex2f(0.05, 0.08);
        glEnd();

		// mouth
		glBegin(GL_LINES);
		glVertex2f(-0.05,  -0.05 );
		glVertex2f( 0.05,  -0.05 );
		glEnd();

		// head
		glBegin(GL_LINE_LOOP);
		glVertex2f( -0.1, -0.1);
		glVertex2f( -0.1, 0.1);
		glVertex2f( 0.1, 0.1);
		glVertex2f( 0.1, -0.1);
		glEnd();
	}

    // called when contacting an object
	void startContact(b2Body* contact) {
        void* contactUD = contact->GetUserData();
        if (!contactUD) {
            ++numContacts;
        }
    }

    // called when no longer contacting an object
	void endContact(b2Body* contact) {
        void* contactUD = contact->GetUserData();
        if (!contactUD) {
            --numContacts;
        }
    }
};

#endif