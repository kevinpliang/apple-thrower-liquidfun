#ifndef CONTACT_H
#define CONTACT_H

#include <Box2D/Box2D.h>
#include "GL/freeglut.h"
#include "Apple.h"
#include "Player.h"

class ContactListener : public b2ContactListener {
    void BeginContact(b2Contact* contact) {  
		//check if fixture A was a player
		void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
		if ( bodyUserData ) {
			static_cast<Entity*>( bodyUserData )->startContact(contact->GetFixtureB()->GetBody());
        }

		//check if fixture B was a player
		bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
		if ( bodyUserData ) {
			static_cast<Entity*>( bodyUserData )->startContact(contact->GetFixtureA()->GetBody());
        }
	}

    void EndContact(b2Contact* contact) {  
        //check if fixture A was a player
        void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
        if ( bodyUserData ) {
            static_cast<Entity*>( bodyUserData )->endContact(contact->GetFixtureB()->GetBody());
        }

        //check if fixture B was a player
        bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
        if ( bodyUserData ) {
            static_cast<Entity*>( bodyUserData )->endContact(contact->GetFixtureA()->GetBody());  
        }
    }
};

#endif