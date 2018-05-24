#include <math.h>
#include <irrlicht.h>

#include "trackball.h"

Trackball::Trackball(const u32 &width, const u32 &height) :
	is_clicked(false),
	is_moving(false)
{
	transform.makeIdentity();
	setBounds(width, height);
}

vector3df Trackball::getVector(const position2df &pos) const
{
	vector3df vect;
	vect.X = 1.0 * pos.X / screen.Width * 2 - 1.0;
	vect.Y = 1.0 * pos.Y / screen.Height * 2 - 1.0;
	vect.Z = 0;

	f32 len = vect.X * vect.X + vect.Y * vect.Y;
	if (len > 1.0f)
		vect.normalize();
	else
		vect.Z = -sqrt(1.0f - len);

	return vect;
}

void Trackball::setBounds(const u32 &width, const u32 &height)
{
	screen = dimension2di(width, height);
}

void Trackball::setDragPos(const s32 &x, const s32 &y)
{
	drag_pos = position2df(x, y);
}

void Trackball::animateNode(ISceneNode *node, u32 timeMs)
{
	if (!node)
		return;

	if (!is_moving)
	{
		if (is_clicked)
		{
			is_moving = true;
			transform = node->getAbsoluteTransformation();
			drag_start = drag_pos;
		}
	}
	else
	{
		if (is_clicked)
		{
			matrix4 rotation;
			quaternion quat;
			vector3df start = getVector(drag_start);
			vector3df end = getVector(drag_pos);
			vector3df perp = start.crossProduct(end);
			if (iszero(perp.getLength()))
			{
				quat.makeIdentity();
			}
			else
			{
				quat.X = perp.X;
				quat.Y = perp.Y;
				quat.Z = perp.Z;
				quat.W = start.dotProduct(end);
			}
			quat.getMatrix(rotation);
			rotation *= transform;
			node->setRotation(rotation.getRotationDegrees());
			node->updateAbsolutePosition();
		}
		else
		{
			is_moving = false;
		}
	}
}
