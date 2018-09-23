#ifndef D_SCENE_H
#define D_SCENE_H

enum
{
	E_SCENE_ID,
	E_SCENE_ID_MODEL,
	E_SCENE_ID_WIELD
};

enum
{
	E_SCENE_AXIS_X,
	E_SCENE_AXIS_Y,
	E_SCENE_AXIS_Z
};

enum
{
	E_SCENE_ID_LIGHT = 0x4000
};

using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

struct ColorSpec
{
	SColor diffuse;
	SColor ambient;
	SColor specular;
};


struct LightSpec
{
	E_LIGHT_TYPE type;
	vector3df position;
	vector3df rotation;
	f32 radius;
	ColorSpec color;
};

class Config;

class LightSource : public ISceneNode
{
public:
	LightSource(ISceneNode *parent, ISceneManager *smgr, s32 id,
		LightSpec lightspec, const wchar_t *text, SColor text_color);
	~LightSource() {}
	void setLight(LightSpec lightspec);
	void setMarker(const bool &is_visible);

	virtual void OnRegisterSceneNode();
	virtual const aabbox3d<f32> &getBoundingBox() const { return box; }
	virtual SMaterial &getMaterial(u32 i) { return material; }
	virtual u32 getMaterialCount() const { return 1; }
	virtual void render();

private:
	ILightSceneNode *light;
	IMeshSceneNode *marker;
	ITextSceneNode *label;
	stringw texture_name;
	SColor line_color;

	aabbox3d<f32> box;
	SMaterial material;
};

class Scene : public ISceneNode
{
public:
	Scene(ISceneNode *parent, ISceneManager *mgr, s32 id);
	~Scene() {}
	bool load(Config *config);
	bool loadModelMesh(const io::path &filename);
	bool loadWieldMesh(const io::path &filename);
	ISceneNode *getNode(s32 id);
	void setAttachment();
	void setAnimation(const u32 &start, const u32 &end, const s32 &speed);
	void setFilter(E_MATERIAL_FLAG flag, const bool &is_enabled);
	void setBackFaceCulling(const bool &is_enabled);
	void setGridColor(SColor color) { grid_color = color; }
	void setGridVisible(const bool &is_visible) { show_grid = is_visible; }
	void setAxesVisible(const bool &is_visible) { show_axes = is_visible; }
	void setLighting(const bool &is_enabled);
	void setLightsVisible(const bool &is_visible);
	void setLightEnabled(s32 index, const bool &is_enabled);
	void setDebugInfo(const bool &is_visible);
	void rotate(s32 axis, const f32 &step);
	void refresh();
	void jump();

	virtual void OnRegisterSceneNode();
	virtual const aabbox3d<f32> &getBoundingBox() const { return box; }
	virtual SMaterial &getMaterial(u32 i) { return material; }
	virtual u32 getMaterialCount() const { return 1; }
	virtual void render();

private:
	void addLights();
	void loadTextures(ISceneNode *node, const std::string &prefix);
	void clearTextures(ISceneNode *node, const std::string &prefix);

	Config *conf;
	bool show_grid;
	bool show_axes;
	SColor grid_color;
	aabbox3d<f32> box;
	SMaterial material;
};

#endif // D_SCENE_H
