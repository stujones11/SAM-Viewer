#ifndef D_VIEWER_H
#define D_VIEWER_H

using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

class Config;
class Scene;
class Trackball;
class GUI;

enum
{
	E_ANIM_STATE_PLAY_FWD,
	E_ANIM_STATE_PLAY_REV,
	E_ANIM_STATE_PAUSED
};

enum
{
	E_PROJECTION_PERSPECTIVE,
	E_PROJECTION_ORTHOGRAPHIC
};

class AnimState
{
public:
	AnimState(IGUIEnvironment *env);
	void load(ISceneNode *node);
	void update(ISceneNode *node);
	void initField(s32 id, const u32 &max, const bool &enabled);
	void setField(s32 id, const u32 &value);
	void setState(s32 id) { state = id; }
	u32 getField(s32 id);
	u32 getFrame() { return frame; }
	s32 getState() { return state; }

private:
	IGUIEnvironment *env;
	u32 frame;
	s32 state;
};

class Viewer : public IEventReceiver
{
public:
	Viewer(Config *conf);
	~Viewer();
	bool run(IrrlichtDevice *irr_device);
	virtual bool OnEvent(const SEvent &event);

private:
	void resize();
	void setProjection();
	void setBackgroundColor(const u32 &color);
	void setCaptionFileName(const io::path &filename);
	void exportStaticMesh(const char *caption, const char **filters,
		const int filter_count, EMESH_WRITER_TYPE id);

	Config *conf;
	IrrlichtDevice *device;
	ICameraSceneNode *camera;
	Scene *scene;
	Trackball *trackball;
	GUI *gui;
	AnimState *animation;
	matrix4 ortho;
	f32 fov;
	f32 fov_home;
	dimension2du screen;
	SColor bg_color;
	u32 jump_time;
};

#endif // D_VIEWER_H
