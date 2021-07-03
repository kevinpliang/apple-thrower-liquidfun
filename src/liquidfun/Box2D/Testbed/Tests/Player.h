#ifndef PLAYER_H
#define PLAYER_H

#include <Box2D/Box2D.h>
#include "GL/freeglut.h"
#include "Apple.h"
#include "Entity.h"

class Player : public Entity {
public: 
    int curr_apples;
	Player(b2World* m_world) : Entity(m_world) {
        my_type = player_type;
        speed = 1.5f;
        entity.position.Set(0.0f, 1.0f);
        body->SetTransform(b2Vec2(0.0f, 2.0f),body->GetAngle());
        max_health = 30;
        curr_health = max_health;
	}

	~Player() {}

    void render() {
        if (numContacts > 0) {
			glColor3f(1, 0, 0); // red
		} else {
			glColor3f(1, 1, 1); // white
		}

		// nose and eyes
		glPointSize(4);
		glBegin(GL_POINTS);
		glVertex2f( 0, 0 );
		glVertex2f(-0.05, 0.05 );
		glVertex2f( 0.05, 0.05 );
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
        if (contactUD) {
            if ( static_cast<Entity*>( contactUD )->my_type == enemy_type ) {
                ++numContacts;
            }
        }
    }

    // called when no longer contacting an object
	void endContact(b2Body* contact) {
        void* contactUD = contact->GetUserData();
        if (contactUD) {
            if ( static_cast<Entity*>( contactUD )->my_type == enemy_type ) {
                --numContacts;
            }
        }
    }
};

#endif