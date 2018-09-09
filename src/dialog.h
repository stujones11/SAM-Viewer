#ifndef D_DIALOG_H
#define D_DIALOG_H

using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

namespace dialog
{
	static const int model_filter_count = 19;
	static const char *model_filters[] = {
		"*.obj", "*.b3d", "*.x", "*.3ds",
		"*.irr", "*.irrmesh", "*.md2", "*.md3",
		"*.bsp", "*.mesh", "*.ms3d", "*.my3D",
		"*.lwo", "*.xml", "*.dae", "*.dmf",
		"*.oct", "*.csm", "*.stl"
	};
	static const int texture_filter_count = 8;
	static const char *texture_filters[] = {
		"*.png", "*.jpg", "*.tga", "*.bmp",
		"*.psd", "*.pcx", "*.ppm", "*.wal"
	};
	extern const char *filename;
	extern bool has_event;
	extern SEvent event;

	void showFileOpen(IGUIEnvironment *env, s32 id, const char *caption,
		const char **filters, const int filter_count);

	void fileOpen(IGUIEnvironment *env, s32 id, const char *caption,
		const char **filters, const int filter_count);
}

enum
{
	E_DIALOG_ID_ABOUT = 0x1000,
	E_DIALOG_ID_SETTINGS,
	E_DIALOG_ID_BG_COLOR,
	E_DIALOG_ID_GRID_COLOR,
	E_DIALOG_ID_COLOR_EDIT,
	E_DIALOG_ID_COLOR_PREVIEW,
	E_DIALOG_ID_WIELD_BONE,
	E_DIALOG_ID_SCREEN_WIDTH,
	E_DIALOG_ID_SCREEN_HEIGHT,
	E_DIALOG_ID_DEBUG_BBOX,
	E_DIALOG_ID_DEBUG_NORMALS,
	E_DIALOG_ID_DEBUG_SKELETON,
	E_DIALOG_ID_DEBUG_WIREFRANE,
	E_DIALOG_ID_DEBUG_ALPHA,
	E_DIALOG_ID_DEBUG_BUFFERS,
	E_DIALOG_ID_ABOUT_OK,
	E_DIALOG_ID_ABOUT_LINK,
	E_DIALOG_ID_SETTINGS_OK,
	E_DIALOG_ID_SETTINGS_CANCEL,
	E_DIALOG_ID_TEXTURES,
	E_DIALOG_ID_TEXTURES_1_MODEL,
	E_DIALOG_ID_TEXTURES_1_WIELD,
	E_DIALOG_ID_TEXTURES_OK,
	E_DIALOG_ID_TEXTURES_CANCEL
};

enum
{
	E_TEXTURE_ID_MODEL = 0x2000,
	E_TEXTURE_ID_WIELD = 0x2010,
	E_BUTTON_ID_MODEL = 0x2020,
	E_BUTTON_ID_WIELD = 0x2030
};

class Config;

class HyperlinkCtrl : public IGUIElement
{
public:
	HyperlinkCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, std::string title, std::string url);
	virtual ~HyperlinkCtrl() {}
	virtual void draw();
	virtual bool OnEvent(const SEvent &event);

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
	std::string getColor() const;
};

class AboutDialog : public IGUIElement
{
public:
	AboutDialog(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle);
	virtual ~AboutDialog() {}
};

class SettingsDialog : public IGUIElement
{
public:
	SettingsDialog(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, Config *conf);
	virtual ~SettingsDialog() {}
	virtual bool OnEvent(const SEvent &event);

private:
	bool isBoxChecked(s32 id) const;
	void colorFromHexStr(const std::string &hex);

	Config *conf;
};

class TexturesDialog : public IGUIElement
{
public:
	TexturesDialog(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, Config *conf, ISceneManager *smgr);
	virtual ~TexturesDialog() {}
	virtual void draw();
	virtual bool OnEvent(const SEvent &event);

private:
	ITexture *getTexture(const io::path &filename);

	Config *conf;
	ISceneManager *smgr;
};

#endif // D_DIALOG_H