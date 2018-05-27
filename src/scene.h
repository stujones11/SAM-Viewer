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

using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

class Config;

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
	void loadTextures(ISceneNode *node, const std::string &prefix);

	Config *conf;
	bool show_grid;
	bool show_axes;
	SColor grid_color;
	aabbox3d<f32> box;
	SMaterial material;
};

#endif // D_SCENE_H
