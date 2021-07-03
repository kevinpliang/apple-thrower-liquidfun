/*
* Copyright (c) 2013 Google, Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/
#ifndef APPLE_H
#define APPLE_H

#include "GL/freeglut.h"
#include "Contact.h"
#include "Entity.h"
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include <cstdio>
#include <vector>
#include <random>

using std::vector;

class Apple : public Test {
public:
	// constructor
	Apple() {
		b2Body* ground = NULL;
		m_world->SetGravity(b2Vec2(0, -9.8));
		m_world->SetContactListener(&contacts);
		{
			b2BodyDef bd;
			ground = m_world->CreateBody(&bd);
		}

		// game bounds
		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.allowSleep = false;
			bd.position.Set(0.0f, 1.0f);
			b2Body* body = m_world->CreateBody(&bd);

			b2PolygonShape shape;
			shape.SetAsBox(0.05f, 1.0f, b2Vec2( 3.0f, 0.0f), 0.0);
			body->CreateFixture(&shape, 5.0f);
			shape.SetAsBox(0.05f, 1.0f, b2Vec2(-3.0f, 0.0f), 0.0);
			body->CreateFixture(&shape, 5.0f);
			// shape.SetAsBox(2.0f, 0.05f, b2Vec2(0.0f, 1.0f), 0.0);
			// body->CreateFixture(&shape, 5.0f);
			shape.SetAsBox(4.0f, 1.0f, b2Vec2(0.0f, -2.0f), 0.0);
			body->CreateFixture(&shape, 5.0f);

			b2RevoluteJointDef jd;
			jd.bodyA = ground;
			jd.bodyB = body;
			jd.localAnchorA.Set(0.0f, 1.0f);
			jd.localAnchorB.Set(0.0f, 0.0f);
			jd.referenceAngle = 0.0f;
			jd.motorSpeed = 0.05f * b2_pi;
			jd.maxMotorTorque = 1e7f;
			jd.enableMotor = true;
			m_joint = (b2RevoluteJoint*)m_world->CreateJoint(&jd);
		}

		// player
		{
			player = new Player(m_world);
			playerAlive = true;	
			player->curr_apples = 50;
		}

		// surf board
		// b2BodyDef board;
		// {			
		// 	board.type = b2_dynamicBody;
		// 	board.allowSleep = false;
		// 	board.position.Set(0.0f, 1.0f);
		// 	b2Body* boardBody = m_world->CreateBody(&board);

		// 	b2PolygonShape boardShape;
		// 	b2FixtureDef boardFixture;
		// 	boardShape.SetAsBox(0.2f, 0.01f, b2Vec2( 0.0f, 2.0f), 0.0);
		// 	boardFixture.shape = &boardShape;
		// 	boardFixture.density = 0.01f;
		// 	boardFixture.friction = 1;
		// 	boardBody->CreateFixture(&boardFixture);
		// }

		// particle system
		m_particleSystem->SetRadius(0.025f);
		const uint32 particleType = TestMain::GetParticleParameterValue();
		m_particleSystem->SetDamping(0.2f);
		{
			b2ParticleGroupDef pd;
			pd.flags = particleType;

			b2PolygonShape shape;
			shape.SetAsBox(0.9f, 0.9f, b2Vec2(0.0f, 1.0f), 0.0);

			pd.shape = &shape;
			b2ParticleGroup * const group = m_particleSystem->CreateParticleGroup(pd);
			if (pd.flags & b2_colorMixingParticle)
			{
				ColorParticleGroup(group, 0);
			}
		}

		m_time = 0;
	}

	void Step(Settings* settings) {
		// setup
		Test::Step(settings);
		if (settings->hz > 0)
		{
			m_time += 1 / settings->hz;
		}

		// text
		m_debugDraw.DrawString(5, m_textLine, "Score: %d", score);
		m_textLine += 50;
		m_debugDraw.DrawString(5, m_textLine, "Health: %d", player->curr_health);
		m_textLine+= 25;
		m_debugDraw.DrawString(5, m_textLine, "Apples: %d", player->curr_apples);
		m_textLine += 150;
		if (playerAlive) {
			m_debugDraw.DrawString(5, m_textLine, "");
		} else {
			m_debugDraw.DrawString(5, m_textLine, "                                  You Died! Press R to Restart");
		}

		// motor for rotation
		m_joint->SetMotorSpeed(0.05f * cosf(m_time) * b2_pi);

		// process entities
		if (playerAlive && !settings->pause) {
			if (randomDouble() < enemyThreshold) {
				Enemy* newEnemy = new Enemy(m_world, enemyHealth);
				enemies.push_back(newEnemy);
				enemyThreshold *= 1.03;
			}

			player->process();
			for (std::size_t i = 0; i < enemies.size(); ++i) {
				Enemy* enemy = enemies[i];
				enemy->process();
				if (enemy->curr_health <= 0) {
					m_world->DestroyBody(enemy->body);
					enemies.erase(enemies.begin()+i);
					score += 10;
					player->curr_apples += 10;
					enemyHealth*=1.05;
				} else {
					enemy->move(player->body->GetPosition());
				}
			}
			if (std::fmod(m_time, 5.0) == 0) {
				player->curr_apples++;
			}

			if (player->curr_health <= 0) {
				playerDeath();
			}
		}
	}

	// called when player dies
	void playerDeath() {
		playerAlive = false;
		m_world->DestroyBody(player->body);	
	}

	// returns random double between (0.0, 1.0) using std::uniform_real_distribution
	double randomDouble() {
		std::uniform_real_distribution<double> distribution(0.0, 1.0);
		return(distribution(generator));
	}

	float32 GetDefaultViewZoom() const {
		return 0.1f;
	}

	static Test* Create() {
		return new Apple;
	}

	void Keyboard(unsigned char key)
	{
		if (playerAlive) {
			switch (key)
			{
			case 'a':
				player->velocity.x = -1 * player->speed;
				break;
			case 's':
				player->velocity.y = -1 * player->speed;
				break;
			case 'd':
				player->velocity.x = player->speed;
				break;
			case 'w':
				player->velocity.y = 2.5f;
				break;
				//player->playerBody->ApplyLinearImpulse( b2Vec2(0,impulse), player->playerBody->GetWorldCenter() , true);
			default:
				Test::Keyboard(key);
				player->velocity.x = 0;
				break;
			}
			player->setVel(player->velocity);
		}
	}

	void MouseDown(const b2Vec2& p)
	{
		if (player->curr_apples > 0 && playerAlive) {
			player->curr_apples--;
			Bullet bullet = Bullet(m_world);
			b2Vec2 playerPos = player->body->GetPosition();
			bullet.setPos(playerPos);
			b2Vec2 moveVector = p - playerPos;
			moveVector.Normalize();
			bullet.setVel(bullet.speed * moveVector);
		}
	}

	void MouseUp(const b2Vec2& p)
	{
		return;
	}

	// Global variables
	Player* player;
	bool playerAlive = true;
	ContactListener contacts;
	vector<Enemy*> enemies;

	std::default_random_engine generator;
	int score = 0;
	double enemyThreshold = 0.01;
	double enemyHealth = 30;

	b2RevoluteJoint* m_joint;
	float32 m_time;

};


#endif
