#ifndef D_TRACKBALL_H
#define D_TRACKBALL_H

using namespace irr;
using namespace core;
using namespace scene;

class Trackball : public ISceneNodeAnimator
{
public:
	Trackball(const u32 &width, const u32 &height);
	~Trackball(void) {}
	void setBounds(const u32 &width, const u32 &height);
	void setDragPos(const s32 &x, const s32 &y);
	void click() { is_clicked = true; }
	void release() { is_clicked = false; }
	bool isClicked() const { return is_clicked; }
	virtual void animateNode(ISceneNode *node, u32 timeMs);
	virtual ISceneNodeAnimator *createClone(ISceneNode *node,
		ISceneManager *newManager = 0) { return 0; }

private:
	vector3df getVector(const position2df &pos) const;

	bool is_clicked;
	bool is_moving;
	position2df drag_start;
	position2df drag_pos;
	dimension2di screen;
	matrix4 transform;
};

#endif // D_TRACKBALL_H
