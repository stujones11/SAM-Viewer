#ifndef D_GUI_H
#define D_GUI_H

using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

enum
{
	E_GUI_ID_MENU,
	E_GUI_ID_TOOLBAR,
	E_GUI_ID_LOAD_MODEL_MESH,
	E_GUI_ID_LOAD_WIELD_MESH,
	E_GUI_ID_SAVE_CONFIG,
	E_GUI_ID_QUIT,
	E_GUI_ID_TEXTURES_DIALOG,
	E_GUI_ID_SETTINGS_DIALOG,
	E_GUI_ID_TOOLBOX_MODEL,
	E_GUI_ID_TOOLBOX_WIELD,
	E_GUI_ID_SHOW_GRID,
	E_GUI_ID_SHOW_AXES,
	E_GUI_ID_ENABLE_WIELD,
	E_GUI_ID_BACK_FACE_CULL,
	E_GUI_ID_ORTHOGONAL,
	E_GUI_ID_PERSPECTIVE,
	E_GUI_ID_BILINEAR,
	E_GUI_ID_TRILINEAR,
	E_GUI_ID_ANISOTROPIC,
	E_GUI_ID_DEBUG_INFO,
	E_GUI_ID_VERTEX,
	E_GUI_ID_VECTOR_X,
	E_GUI_ID_VECTOR_Y,
	E_GUI_ID_VECTOR_Z,
	E_GUI_ID_POSITION,
	E_GUI_ID_ROTATION,
	E_GUI_ID_SCALE,
	E_GUI_ID_MATERIAL,
	E_GUI_ID_ANIM_CTRL,
	E_GUI_ID_ANIM_START,
	E_GUI_ID_ANIM_END,
	E_GUI_ID_ANIM_FRAME,
	E_GUI_ID_ANIM_SPEED,
	E_GUI_ID_SKIP_REV,
	E_GUI_ID_PLAY_REV,
	E_GUI_ID_PAUSE,
	E_GUI_ID_PLAY_FWD,
	E_GUI_ID_SKIP_FWD
};

class Config;

class VertexCtrl : public IGUIElement
{
public:
	VertexCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, f32 step, const wchar_t *label);
	virtual ~VertexCtrl() {}
	virtual bool OnEvent(const SEvent &event);
	f32 getValue() const { return vertex; }
	void setValue(const f32 &value);

private:
	f32 vertex;
};

class VectorCtrl : public IGUIElement
{
public:
	VectorCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, f32 step, const wchar_t *label);
	virtual ~VectorCtrl() {}
	virtual bool OnEvent(const SEvent &event);
	vector3df getVector() const { return vector; }
	void setVector(const vector3df &vec);

private:
	vector3df vector;
};

class AnimCtrl : public IGUIElement
{
public:
	AnimCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle);
	virtual ~AnimCtrl() {}
	virtual bool OnEvent(const SEvent &event);
	void reset(bool enabled);

private:
	s32 button_id;
};

class ToolBox : public IGUIElement
{
public:
	ToolBox(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, ISceneNode *node);
	virtual ~ToolBox() {}
	virtual bool OnEvent(const SEvent &event);

private:
	s32 node_id;
	ISceneManager *smgr;
};

class GUI
{
public:
	GUI(IrrlichtDevice *device, Config *config);
	void initMenu();
	void initToolBar();
	void showToolBox(s32 id);
	void closeToolBox(s32 id);
	void reloadToolBox(s32 id);
	IGUIElement *getElement(s32 id);
	bool getFocused() const { return has_focus; }
	void setFocused(const bool &focus) { has_focus = focus; }
	void moveElement(s32 id, const vector2di &move);
	void showTexturesDialog();
	void showSettingsDialog();
	void showAboutDialog();

private:
	const rect<s32> getWindowRect(const u32 &width, const u32 &height) const;

	IrrlichtDevice *device;
	Config *conf;
	bool has_focus;
};

#endif // D_GUI_H