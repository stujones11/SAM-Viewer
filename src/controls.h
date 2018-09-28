#ifndef D_CONTROLS_H
#define D_CONTROLS_H

using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

enum
{
	E_CTRL_ID_VERTEX = 0x3000,
	E_CTRL_ID_VECTOR_X,
	E_CTRL_ID_VECTOR_Y,
	E_CTRL_ID_VECTOR_Z,
	E_CTRL_ID_SKIP_REV,
	E_CTRL_ID_PLAY_REV,
	E_CTRL_ID_PAUSE,
	E_CTRL_ID_PLAY_FWD,
	E_CTRL_ID_SKIP_FWD,
	E_CTRL_ID_COLOR_EDIT,
	E_CTRL_ID_COLOR_PREVIEW,
	E_CTRL_ID_COLOR_CHOOSER,
	E_CTRL_ID_COLOR_RED,
	E_CTRL_ID_COLOR_BLUE,
	E_CTRL_ID_COLOR_GREEN,
	E_CTRL_ID_COLOR_HUE,
	E_CTRL_ID_COLOR_SAT,
	E_CTRL_ID_COLOR_LUM,
	E_CTRL_ID_COLOR_OK,
	E_CTRL_ID_COLOR_CANCEL
};


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
	std::string getString() const;
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

class HyperlinkCtrl : public IGUIElement
{
public:
	HyperlinkCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, std::string title, std::string url);
	virtual ~HyperlinkCtrl() {}
	virtual void draw();
	virtual bool OnEvent(const SEvent &event);
	void openBrowser(std::string url);

private:
	std::string url;
	bool is_active;
};

class ColorCtrl : public IGUIElement
{
public:
	ColorCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, const wchar_t *label);
	virtual ~ColorCtrl() {}
	virtual bool OnEvent(const SEvent &event);
	void setColor(const std::string &hex);
	void setColor(const SColor &color);
	std::string getString() const;
	SColor getColor() const;
	bool isValidHexString(std::string hex);
};

class ColorChooser : public IGUIElement
{
public:
	ColorChooser(IGUIEnvironment *env, IGUIElement *parent,
		ColorCtrl *receiver, s32 id, const rect<s32> &rectangle,
		SColor color_orig);
	virtual ~ColorChooser() {}
	virtual void draw();
	virtual bool OnEvent(const SEvent &event);

private:
	void setColor();
	SColor getPixelColor(const vector2di &pos);

	ColorCtrl *receiver;
	SColor color_orig;
	SColor color_selected;
	SColorHSL color_hsl;
	rect<s32> rect_color;
	rect<s32> rect_shade;
	rect<s32> rect_orig;
};

#endif // D_CONTROLS_H