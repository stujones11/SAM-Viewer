#include <stdlib.h>
#include <iostream>
#include <irrlicht.h>

#include "config.h"
#include "scene.h"

LightSource::LightSource(ISceneNode *parent, ISceneManager *smgr, s32 id,
		LightSpec lightspec, const wchar_t *text, SColor text_color) :
	ISceneNode(parent, smgr, id),
	light(0),
	marker(0),
	label(0)
{
	line_color = text_color;
	line_color.setAlpha(64);
	text_color.setAlpha(255);
	texture_name = "light_";
	texture_name.append(text);

	material.Lighting = false;
	material.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
	material.BackfaceCulling = false;

	IVideoDriver *driver = SceneManager->getVideoDriver();
	ITexture *texture = driver->addTexture(dimension2d<u32>(1,1),
		texture_name.c_str());

	marker = smgr->addSphereSceneNode(0.75f, 12, this);
	marker->setMaterialFlag(EMF_LIGHTING, false);
	marker->setMaterialTexture(0, texture);

	IGUIEnvironment *env = smgr->getGUIEnvironment();
	IGUIFont *font = env->getFont("fontlucida.png");
	label = smgr->addTextSceneNode(font, text, text_color, this);
	label->setMaterialFlag(EMF_LIGHTING, false);

	light = smgr->addLightSceneNode(this);
	setLight(lightspec);
}

void LightSource::setLight(LightSpec lightspec)
{
	setPosition(lightspec.position);
	setRotation(lightspec.rotation);
	SLight data;
	data.DiffuseColor = lightspec.color.diffuse;
	data.AmbientColor = lightspec.color.ambient;
	data.SpecularColor = lightspec.color.specular;
	light->setLightData(data);
	light->setLightType(lightspec.type);
	light->setRadius(lightspec.radius);

	IVideoDriver *driver = SceneManager->getVideoDriver();
	ITexture *texture = driver->getTexture(texture_name.c_str());
	if (texture)
	{
		s32 *p = (s32*)texture->lock();
		p[0] = lightspec.color.diffuse.color;
		texture->unlock();
	}
}

void LightSource::setMarker(const bool &is_visible)
{
	marker->setVisible(is_visible);
	label->setVisible(is_visible);
}

void LightSource::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

void LightSource::render()
{
	if (!marker->isVisible())
		return;

	IVideoDriver *driver = SceneManager->getVideoDriver();
	driver->setMaterial(material);
	driver->setTransform(ETS_WORLD, AbsoluteTransformation);
	driver->draw3DLine(vector3df(0,0,0), vector3df(0,0,100), line_color);
}

Scene::Scene(ISceneNode *parent, ISceneManager *smgr, s32 id) :
	ISceneNode(parent, smgr, id),
	conf(0),
	show_grid(true),
	show_axes(true)
{
	material.Lighting = false;
	material.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
	material.BackfaceCulling = false;
	grid_color = SColor(64,128,128,128);
}

bool Scene::load(Config *config)
{
	conf = config;
	if (!loadModelMesh(conf->getCStr("model_mesh")))
		return false;

	setAttachment();
	loadWieldMesh(conf->getCStr("wield_mesh"));
	setBackFaceCulling(conf->getBool("backface_cull"));
	setGridColor(conf->getHex("grid_color"));
	addLights();
	setLighting(conf->getBool("lighting"));
	return true;
}

bool Scene::loadModelMesh(const io::path &filename)
{
	if (!conf)
		return false;

	IAnimatedMesh *mesh = SceneManager->getMesh(filename);
	if (!mesh)
		return false;

	ISceneNode *wield = getNode(E_SCENE_ID_WIELD);
	if (wield)
		wield->setParent(this);

	ISceneNode *model = getNode(E_SCENE_ID_MODEL);
	if (model)
	{
		model->remove();
		model = 0;
	}
	model = SceneManager->addAnimatedMeshSceneNode(mesh, this,
		E_SCENE_ID_MODEL);
	if (!model)
		return false;

	Vector pos = conf->getVector("model_position");
	Vector rot = conf->getVector("model_rotation");
	f32 s = (f32)conf->getInt("model_scale") / 100;
	u32 mat = conf->getInt("model_material");

	model->setMaterialFlag(EMF_LIGHTING, conf->getBool("lighting"));
	model->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	model->setPosition(vector3df(pos.x, pos.y, pos.z));
	model->setRotation(vector3df(rot.x, rot.y, rot.z));
	model->setScale(vector3df(s,s,s));
	model->setMaterialType((E_MATERIAL_TYPE)mat);
	model->setMaterialFlag(EMF_BILINEAR_FILTER,
		conf->getBool("bilinear"));
	model->setMaterialFlag(EMF_TRILINEAR_FILTER,
		conf->getBool("trilinear"));
	model->setMaterialFlag(EMF_ANISOTROPIC_FILTER,
		conf->getBool("anisotropic"));

	setDebugInfo(conf->getBool("debug_info"));
	if (wield)
		setAttachment();

	loadTextures(model, "model");
	return true;
}

bool Scene::loadWieldMesh(const io::path &filename)
{
	if (!conf)
		return false;

	IMesh *mesh = SceneManager->getMesh(filename);
	if (!mesh)
		return false;

	ISceneNode *wield = getNode(E_SCENE_ID_WIELD);
	if (wield)
	{
		wield->remove();
		wield = 0;
	}
	wield = SceneManager->addMeshSceneNode(mesh, this,
		E_SCENE_ID_WIELD);

	if (!wield)
		return false;

	Vector pos = conf->getVector("wield_position");
	Vector rot = conf->getVector("wield_rotation");
	f32 s = (f32)conf->getInt("wield_scale") / 100;
	u32 mat = conf->getInt("wield_material");

	wield->setMaterialFlag(EMF_LIGHTING, conf->getBool("lighting"));
	wield->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	wield->setPosition(vector3df(pos.x, pos.y, pos.z));
	wield->setRotation(vector3df(rot.x, rot.y, rot.z));
	wield->setScale(vector3df(s,s,s));
	wield->setVisible(conf->getBool("wield_show"));
	wield->setMaterialType((E_MATERIAL_TYPE)mat);
	wield->setMaterialFlag(EMF_BILINEAR_FILTER,
		conf->getBool("bilinear"));
	wield->setMaterialFlag(EMF_TRILINEAR_FILTER,
		conf->getBool("trilinear"));
	wield->setMaterialFlag(EMF_ANISOTROPIC_FILTER,
		conf->getBool("anisotropic"));
	wield->setVisible(conf->getBool("wield_show"));

	setAttachment();
	loadTextures(wield, "wield");
	return true;
}

void Scene::addLights()
{
	for (int i = 0; i < 3; ++i)
	{
		std::string n = std::to_string(i + 1);
		Vector pos = conf->getVector("light_position_" + n);
		Vector rot =  conf->getVector("light_rotation_" + n);
		LightSpec lightspec;
		lightspec.type = (E_LIGHT_TYPE)conf->getInt("light_type_" + n);
		lightspec.position = vector3df(pos.x, pos.y, pos.z);
		lightspec.rotation = vector3df(rot.x, rot.y, rot.z);
		lightspec.color.diffuse = conf->getHex("light_color_diffuse_" + n);
		lightspec.color.ambient = conf->getHex("light_color_ambient_" + n);
		lightspec.color.specular = conf->getHex("light_color_specular_" + n);
		lightspec.radius = conf->getInt("light_radius_" + n);
		SColor text_color = conf->getHex("grid_color");
		stringw label = n.c_str();
		LightSource *light = new LightSource(this, SceneManager,
			E_SCENE_ID_LIGHT + i, lightspec, label.c_str(), text_color);
		light->drop();
	}
}

void Scene::setLighting(const bool &is_enabled)
{	ISceneNode *model = getNode(E_SCENE_ID_MODEL);
	if (model)
		model->setMaterialFlag(EMF_LIGHTING, is_enabled);

	ISceneNode *wield = getNode(E_SCENE_ID_WIELD);
	if (wield)
		wield->setMaterialFlag(EMF_LIGHTING, is_enabled);
	for (int i = 0; i < 3; ++i)
	{
		std::string key = "light_enabled_" + std::to_string(i + 1);
		setLightEnabled(i, conf->getBool(key) && is_enabled);
	}
}

void Scene::setLightsVisible(const bool &is_visible)
{
	for (int i = 0; i < 3; ++i)
	{
		LightSource *light = (LightSource*)
			SceneManager->getSceneNodeFromId(E_SCENE_ID_LIGHT + i);
		light->setMarker(is_visible);
	}
}

void Scene::setLightEnabled(s32 index, const bool &is_enabled)
{
	LightSource *light = (LightSource*)
		SceneManager->getSceneNodeFromId(E_SCENE_ID_LIGHT + index);
	light->setVisible(is_enabled);
}

void Scene::clearTextures(ISceneNode *node, const std::string &prefix)
{
	for (u32 i = 0; i < node->getMaterialCount(); ++i)
	{
		SMaterial &material = node->getMaterial(i);
		material.TextureLayer[0].Texture = 0;
	}
}

void Scene::loadTextures(ISceneNode *node, const std::string &prefix)
{
	IVideoDriver *driver = SceneManager->getVideoDriver();
	u32 material_count = node->getMaterialCount();
	u32 texture_count = (material_count < 6) ? material_count : 5;
	if (conf->getBool(prefix + "_texture_single"))
	{
		io::path fn = conf->getCStr(prefix + "_texture_1");
		ITexture *texture = driver->getTexture(fn);
		if (texture)
		{
			for (u32 i = 0; i < material_count; ++i)
			{
				SMaterial &material = node->getMaterial(i);
				material.TextureLayer[0].Texture = texture;
			}
		}
	}
	else
	{
		for (u32 i = 0; i < texture_count; ++i)
		{
			std::string key = prefix + "_texture_" + std::to_string(i + 1);
			io::path fn = conf->getCStr(key);
			ITexture *texture = driver->getTexture(fn);
			if (texture)
			{
				SMaterial &material = node->getMaterial(i);
				material.TextureLayer[0].Texture = texture;
			}
		}
	}
}

ISceneNode *Scene::getNode(s32 id)
{
	return SceneManager->getSceneNodeFromId(id);
}

void Scene::setAttachment()
{
	if (!conf)
		return;

	IAnimatedMeshSceneNode *model =
		(IAnimatedMeshSceneNode*)getNode(E_SCENE_ID_MODEL);
	ISceneNode *wield = getNode(E_SCENE_ID_WIELD);
	if (!model || !wield)
		return;

	IBoneSceneNode *bone = 0;
	if (model->getJointCount() > 0)
	{
		stringc wield_bone = conf->getCStr("wield_bone");
		bone = model->getJointNode(wield_bone.c_str());
	}
	if (bone)
		wield->setParent(bone);
	else
		wield->setParent(model);
}

void Scene::setAnimation(const u32 &start, const u32 &end, const s32 &speed)
{
	if (!conf)
		return;

	IAnimatedMeshSceneNode *model =
		(IAnimatedMeshSceneNode*)getNode(E_SCENE_ID_MODEL);

	if (model)
	{
		model->setFrameLoop(start, end);
		model->setAnimationSpeed(speed);
	}
}

void Scene::setFilter(E_MATERIAL_FLAG flag, const bool &is_enabled)
{
	ISceneNode *model = getNode(E_SCENE_ID_MODEL);
	if (model)
		model->setMaterialFlag(flag, is_enabled);

	ISceneNode *wield = getNode(E_SCENE_ID_WIELD);
	if (wield)
		wield->setMaterialFlag(flag, is_enabled);
}

void Scene::setBackFaceCulling(const bool &is_enabled)
{
	ISceneNode *model = getNode(E_SCENE_ID_MODEL);
	if (model)
		model->setMaterialFlag(EMF_BACK_FACE_CULLING, is_enabled);

	ISceneNode *wield = getNode(E_SCENE_ID_WIELD);
	if (wield)
		wield->setMaterialFlag(EMF_BACK_FACE_CULLING, is_enabled);
}

void Scene::setDebugInfo(const bool &is_visible)
{
	ISceneNode *model = getNode(E_SCENE_ID_MODEL);
	if (model)
	{
		u32 state = (is_visible) ? conf->getInt("debug_flags") : EDS_OFF;
		model->setDebugDataVisible(state);
	}
}

void Scene::rotate(s32 axis, const f32 &step)
{
	matrix4 m, n;
	m.setRotationDegrees(getRotation());
	switch (axis)
	{
	case E_SCENE_AXIS_X:
		n.setRotationDegrees(vector3df(step,0,0));
		break;
	case E_SCENE_AXIS_Y:
		n.setRotationDegrees(vector3df(0,step,0));
		break;
	case E_SCENE_AXIS_Z:
		n.setRotationDegrees(vector3df(0,0,step));
		break;
	}
	m *= n;
	setRotation(m.getRotationDegrees());
}

void Scene::refresh()
{
	IVideoDriver *driver = SceneManager->getVideoDriver();

	// Important, clear all texture refs before removing.
	ISceneNode *model = getNode(E_SCENE_ID_MODEL);
	ISceneNode *wield = getNode(E_SCENE_ID_WIELD);
	if (model)
		clearTextures(model, "model");
	if (wield)
		clearTextures(wield, "wield");

	// Remove all textures before reloading.
	std::string prefix[] = {"model", "wield"};
	for (u32 p = 0; p < 2; ++p)
	{
		for (u32 i = 0; i < 6; ++i)
		{
			std::string key = prefix[p] + "_texture_" + std::to_string(i + 1);
			if (conf->hasKey(key))
			{
				io::path fn = conf->getCStr(key);
				ITexture *texture = driver->getTexture(fn);
				if (texture)
					driver->removeTexture(texture);
			}
		}
	}
	if (model)
		loadTextures(model, "model");
	if (wield)
		loadTextures(wield, "wield");
}

void Scene::jump()
{
	// quick and dirty jump animation to test attachment inertia
	// this should eventually be improved or removed
	ISceneNode *model = getNode(E_SCENE_ID_MODEL);
	if (!model)
		return;

	Vector p = conf->getVector("model_position");
	vector3df start = vector3df(p.x,p.y,p.z);
	vector3df end = start + vector3df(0,10,0);
	ISceneNodeAnimator *anim =
		SceneManager->createFlyStraightAnimator(start, end, 380, false, true);
	model->removeAnimators();
	model->addAnimator(anim);
	anim->drop();
}

void Scene::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

void Scene::render()
{
	if (!show_grid)
		return;

	IVideoDriver *driver = SceneManager->getVideoDriver();
	driver->setMaterial(material);
	driver->setTransform(ETS_WORLD, AbsoluteTransformation);

	SColor grid = grid_color;
	grid.setAlpha(64);
	for (f32 n = -10; n <= 10; n += 2)
	{
		driver->draw3DLine(vector3df(n,0,-10), vector3df(n,-0,10), grid);
		driver->draw3DLine(vector3df(-10,0,n), vector3df(10,-0,n), grid);
	}
	if (!show_axes)
		return;

	SColor red(128,255,0,0);
	SColor green(128,0,255,0);
	SColor blue(128,0,0,255);

	driver->draw3DLine(vector3df(-10,0,-10), vector3df(-8,0,-10), red);
	driver->draw3DLine(vector3df(-8,0,-10), vector3df(-8.5,0,-9.8), red);
	driver->draw3DLine(vector3df(-8,0,-10), vector3df(-8.5,0,-10.2), red);

	driver->draw3DLine(vector3df(-10,0,-10), vector3df(-10,2,-10), green);
	driver->draw3DLine(vector3df(-10,2,-10), vector3df(-9.8,1.5,-10), green);
	driver->draw3DLine(vector3df(-10,2,-10), vector3df(-10.2,1.5,-10), green);

	driver->draw3DLine(vector3df(-10,0,-10), vector3df(-10,0,-8), blue);
	driver->draw3DLine(vector3df(-10,0,-8), vector3df(-9.8,0,-8.5), blue);
	driver->draw3DLine(vector3df(-10,0,-8), vector3df(-10.2,0,-8.5), blue);

	IGUIEnvironment *env = SceneManager->getGUIEnvironment();
	IGUIFont *font = env->getFont("fontlucida.png");
	if (font)
	{
		s32 btm = driver->getScreenSize().Height;
		s32 top = btm - 20;
		font->draw(L"X", rect<s32>(5,top,15,btm), red);
		font->draw(L"Y", rect<s32>(15,top,25,btm), green);
		font->draw(L"Z", rect<s32>(25,top,35,btm), blue);
	}
}
