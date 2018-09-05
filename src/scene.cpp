#include <stdlib.h>
#include <iostream>
#include <irrlicht.h>

#include "config.h"
#include "scene.h"

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

	model->setMaterialFlag(EMF_LIGHTING, false);
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

	wield->setMaterialFlag(EMF_LIGHTING, false);
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
