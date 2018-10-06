#ifndef D_DIALOG_H
#define D_DIALOG_H

using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

enum
{
	E_DIALOG_ID_ABOUT = 0x1000,
	E_DIALOG_ID_SETTINGS,
	E_DIALOG_ID_BG_COLOR,
	E_DIALOG_ID_GRID_COLOR,
	E_DIALOG_ID_WIELD_BONE,
	E_DIALOG_ID_SCREEN_WIDTH,
	E_DIALOG_ID_SCREEN_HEIGHT,
	E_DIALOG_ID_DEBUG_BBOX,
	E_DIALOG_ID_DEBUG_NORMALS,
	E_DIALOG_ID_DEBUG_SKELETON,
	E_DIALOG_ID_DEBUG_WIREFRANE,
	E_DIALOG_ID_DEBUG_ALPHA,
	E_DIALOG_ID_DEBUG_BUFFERS,
	E_DIALOG_ID_EXPORT_ANIM,
	E_DIALOG_ID_EXPORT_TRANSFORM,
	E_DIALOG_ID_EXPORT_FLIP,
	E_DIALOG_ID_EXPORT_NORMAL,
	E_DIALOG_ID_EXPORT_COMBINE,
	E_DIALOG_ID_EXPORT_SCALE,
	E_DIALOG_ID_ABOUT_OK,
	E_DIALOG_ID_ABOUT_LINK,
	E_DIALOG_ID_SETTINGS_OK,
	E_DIALOG_ID_SETTINGS_CANCEL,
	E_DIALOG_ID_TEXTURES,
	E_DIALOG_ID_TEXTURES_1_MODEL,
	E_DIALOG_ID_TEXTURES_1_WIELD,
	E_DIALOG_ID_TEXTURES_OK,
	E_DIALOG_ID_TEXTURES_CANCEL,
	E_DIALOG_ID_LIGHTS,
	E_DIALOG_ID_LIGHTS_INIT,
	E_DIALOG_ID_LIGHTS_PREVIEW,
	E_DIALOG_ID_LIGHTS_OK,
	E_DIALOG_ID_LIGHTS_CANCEL
};

enum
{
	E_TEXTURE_ID_MODEL = 0x2000,
	E_TEXTURE_ID_WIELD = 0x2010,
	E_BUTTON_ID_MODEL = 0x2020,
	E_BUTTON_ID_WIELD = 0x2030,
	E_DIALOG_ID_LIGHT_TYPE = 0x2040,
	E_DIALOG_ID_LIGHT_POS = 0x2050,
	E_DIALOG_ID_LIGHT_ROT = 0x2060,
	E_DIALOG_ID_LIGHT_DIFFUSE = 0x2070,
	E_DIALOG_ID_LIGHT_AMBIENT = 0x2080,
	E_DIALOG_ID_LIGHT_SPECULAR = 0x2090,
	E_DIALOG_ID_LIGHT_RADIUS = 0x2100
};

enum
{
	E_MESH_EXPORT_ANIM = 1,
	E_MESH_EXPORT_TRANSFORM = 2,
	E_MESH_EXPORT_FLIP = 4,
	E_MESH_EXPORT_NORMAL = 8
};

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

	const char *fileOpenDialog(io::IFileSystem *fs, const char *caption,
		const char **filters, const int filter_count);

	const char *fileSaveDialog(io::IFileSystem *fs, const char *caption,
		const char **filters, const int filter_count);
}

class Config;

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
	virtual bool OnEvent(const SEvent &event);

private:
	ITexture *getTexture(const io::path &filename);

	Config *conf;
	ISceneManager *smgr;
};

class LightsDialog : public IGUIElement
{
public:
	LightsDialog(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, Config *conf, ISceneManager *smgr);
	virtual ~LightsDialog() {}
	virtual bool OnEvent(const SEvent &event);

private:
	void setLights(s32 id);

	Config *conf;
	ISceneManager *smgr;
};

#endif // D_DIALOG_H