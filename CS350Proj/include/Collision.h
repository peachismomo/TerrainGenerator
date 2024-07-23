#ifndef COLLISION_H
#define COLLISION_H

#include "includes.h"
#include <Object.h>

class Collision
{
public:
	void Init();
	void Update();
	void End();
private:
	std::vector<Object*>* m_objects;

};

#endif // !COLLISION_H
