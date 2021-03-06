#include <stdlib.h>
#include <iostream>
#include <irrlicht.h>

#include "config.h"
#include "scene.h"
#include "controls.h"
#include "tinyfiledialogs.h"
#include "dialog.h"

#ifdef USE_CMAKE_CONFIG_H
#include "cmake_config.h"
#else
#define D_ABOUT_LINK_URL "https://github.com/stujones11/SAM-Viewer"
#define D_ABOUT_LINK_TEXT "github.com/stujones11/SAM-Viewer"
#define D_VERSION "dirty"
#endif

namespace dialog
{
	static inline void setWorkingDir(io::IFileSystem *fs, const char *fn)
	{
		if (!fn || stringc(fn).empty())
			return;
		io::path cwd = fs->getFileDir(fn);
		fs->changeWorkingDirectoryTo(cwd);
	}

	const char *fileOpenDialog(io::IFileSystem *fs, const char *caption,
		const char **filters, const int filter_count)
	{
		io::path path = fs->getWorkingDirectory() + "/";
		const char *fn = tinyfd_openFileDialog(caption, path.c_str(),
			filter_count, filters, 0);
		setWorkingDir(fs, fn);
		return fn;
	}

	const char *fileSaveDialog(io::IFileSystem *fs, const char *caption,
		const char **filters, const int filter_count)
	{
		io::path path = fs->getWorkingDirectory() + "/";
		const char *fn = tinyfd_saveFileDialog(caption, path.c_str(),
			filter_count, filters);
		setWorkingDir(fs, fn);
		return fn;
	}
}

AboutDialog::AboutDialog(IGUIEnvironment *env, IGUIElement *parent,
	s32 id, const rect<s32> &rectangle) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle)
{
	IVideoDriver *driver = env->getVideoDriver();
	ITexture *icon = driver->findTexture("sam_icon_128.png");
	if (!icon)
		icon = driver->getTexture("sam_icon_128.png");
	if (icon)
	{
		IGUIImage *image = env->addImage(rect<s32>(86,10,214,138), this);
		image->setImage(icon);
	}
	ITexture *title = driver->findTexture("title.png");
	if (!title)
		title = driver->getTexture("title.png");
	if (title)
	{
		IGUIImage *image = env->addImage(rect<s32>(50,140,250,170), this);
		image->setImage(title);
	}
	stringw desc = stringw("Skin & Model Viewer - Version ") + D_VERSION;
	IGUIStaticText *text;
	text = env->addStaticText(desc.c_str(), rect<s32>(20,175,280,195),
		false, false, this);
	text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);

	HyperlinkCtrl *link = new HyperlinkCtrl(env, this, E_DIALOG_ID_ABOUT_LINK,
		rect<s32>(32,200,268,216), D_ABOUT_LINK_TEXT, D_ABOUT_LINK_URL);
	link->drop();

	env->addButton(rect<s32>(110,235,190,265), this,
		E_DIALOG_ID_ABOUT_OK, L"OK");
}

SettingsDialog::SettingsDialog(IGUIEnvironment *env, IGUIElement *parent,
	s32 id, const rect<s32> &rectangle, Config *conf) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
	conf(conf)
{
	IGUITabControl *tabs = env->addTabControl(rect<s32>(2,2,398,250),
		this, true, true);

	IGUITab *tab_general = tabs->addTab(L"General");
	IGUITab *tab_debug = tabs->addTab(L"Debug");
	IGUITab *tab_export = tabs->addTab(L"Export");
	IGUISpinBox *spin;
	IGUICheckBox *check;
	ColorCtrl *color;

	color = new ColorCtrl(env, tab_general, E_DIALOG_ID_BG_COLOR,
		rect<s32>(20,20,320,40), L"Background Color:");
	color->setColor(conf->get("bg_color"));
	color->drop();

	color = new ColorCtrl(env, tab_general, E_DIALOG_ID_GRID_COLOR,
		rect<s32>(20,50,320,70), L"Grid Color:");
	color->setColor(conf->get("grid_color"));
	color->drop();

	env->addStaticText(L"Wield Attachment Bone:", rect<s32>(20,80,180,100),
		false, false, tab_general, -1);
	stringw bone_name = conf->getCStr("wield_bone");
	env->addEditBox(bone_name.c_str(), rect<s32>(200,80,320,100),
		true, tab_general, E_DIALOG_ID_WIELD_BONE);

	env->addStaticText(L"Default Screen Width:",
		rect<s32>(20,110,180,130), false, false, tab_general, -1);
	spin = env->addSpinBox(L"", rect<s32>(200,110,270,130),
		true, tab_general, E_DIALOG_ID_SCREEN_WIDTH);
	spin->setValue(conf->getInt("screen_width"));
	spin->setDecimalPlaces(0);

	env->addStaticText(L"Default Screen Height:",
		rect<s32>(20,140,180,160), false, false, tab_general, -1);
	spin = env->addSpinBox(L"", rect<s32>(200,140,270,160),
		true, tab_general, E_DIALOG_ID_SCREEN_HEIGHT);
	spin->setValue(conf->getInt("screen_height"));
	spin->setDecimalPlaces(0);

	check = env->addCheckBox(false, rect<s32>(20,20,380,40), tab_debug,
		E_DIALOG_ID_DEBUG_BBOX, L"Show bounding boxes");
	check->setChecked(conf->getInt("debug_flags") & EDS_BBOX);
	check = env->addCheckBox(false, rect<s32>(20,50,380,70), tab_debug,
		E_DIALOG_ID_DEBUG_NORMALS, L"Show vertex normals");
	check->setChecked(conf->getInt("debug_flags") & EDS_NORMALS);
	check = env->addCheckBox(false, rect<s32>(20,80,380,100), tab_debug,
		E_DIALOG_ID_DEBUG_SKELETON, L"Show skeleton");
	check->setChecked(conf->getInt("debug_flags") & EDS_SKELETON);
	check = env->addCheckBox(false, rect<s32>(20,110,380,130), tab_debug,
		E_DIALOG_ID_DEBUG_WIREFRANE, L"Wireframe overlay");
	check->setChecked(conf->getInt("debug_flags") & EDS_MESH_WIRE_OVERLAY);
	check = env->addCheckBox(false, rect<s32>(20,140,380,160), tab_debug,
		E_DIALOG_ID_DEBUG_ALPHA, L"Use transparent material");
	check->setChecked(conf->getInt("debug_flags") & EDS_HALF_TRANSPARENCY);
	check = env->addCheckBox(false, rect<s32>(20,170,380,190),	tab_debug,
		E_DIALOG_ID_DEBUG_BUFFERS, L"Show all mesh buffers");
	check->setChecked(conf->getInt("debug_flags") & EDS_BBOX_BUFFERS);

	check = env->addCheckBox(false, rect<s32>(20,20,380,40), tab_export,
		E_DIALOG_ID_EXPORT_ANIM, L"Apply animation pose");
	check->setChecked(conf->getInt("export_flags") & E_MESH_EXPORT_ANIM);
	check = env->addCheckBox(false, rect<s32>(20,50,380,70), tab_export,
		E_DIALOG_ID_EXPORT_TRANSFORM, L"Apply viewer transformations");
	check->setChecked(conf->getInt("export_flags") & E_MESH_EXPORT_TRANSFORM);
	check = env->addCheckBox(false, rect<s32>(20,80,380,100), tab_export,
		E_DIALOG_ID_EXPORT_FLIP, L"Flip surfaces");
	check->setChecked(conf->getInt("export_flags") & E_MESH_EXPORT_FLIP);
	check = env->addCheckBox(false, rect<s32>(20,110,380,130), tab_export,
		E_DIALOG_ID_EXPORT_NORMAL, L"Recalculate normals");
	check->setChecked(conf->getInt("export_flags") & E_MESH_EXPORT_NORMAL);

	env->addStaticText(L"Scale:", rect<s32>(20,140,80,160),
		false, false, tab_export);
	env->addStaticText(L"%", rect<s32>(80,140,100,160),
		false, false, tab_export);
	spin = env->addSpinBox(L"", rect<s32>(100,140,180,160), false, tab_export,
		E_DIALOG_ID_EXPORT_SCALE);
	spin->setRange(0, 10000);
	spin->setValue(conf->getInt("export_scale"));
	spin->setDecimalPlaces(0);

	env->addButton(rect<s32>(315,255,395,285), this,
		E_DIALOG_ID_SETTINGS_OK, L"OK");
	env->addButton(rect<s32>(230,255,310,285), this,
		E_DIALOG_ID_SETTINGS_CANCEL, L"Cancel");
}

bool SettingsDialog::isBoxChecked(s32 id) const
{
	IGUICheckBox *check = (IGUICheckBox*)getElementFromId(id, true);
	if (check)
		return check->isChecked();

	return false;
}

bool SettingsDialog::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT &&
		event.GUIEvent.EventType == EGET_BUTTON_CLICKED &&
		event.GUIEvent.Caller->getID() == E_DIALOG_ID_SETTINGS_OK)
	{
		IGUIEditBox *edit;
		IGUISpinBox *spin;
		ColorCtrl *color;

		color = (ColorCtrl*)getElementFromId(E_DIALOG_ID_BG_COLOR, true);
		if (color)
		{
			const std::string hex = color->getString();
			if (color->isValidHexString(hex))
				conf->set("bg_color", hex);
		}
		color = (ColorCtrl*)getElementFromId(E_DIALOG_ID_GRID_COLOR, true);
		if (color)
		{
			const std::string hex = color->getString();
			if (color->isValidHexString(hex))
				conf->set("grid_color", hex);
		}
		edit = (IGUIEditBox*)
			getElementFromId(E_DIALOG_ID_WIELD_BONE, true);
		std::string bone = stringc(edit->getText()).c_str();
		conf->set("wield_bone", bone);

		spin = (IGUISpinBox*)
			getElementFromId(E_DIALOG_ID_SCREEN_WIDTH, true);
		u32 width = spin->getValue();
		conf->set("screen_width", std::to_string(width));
		spin = (IGUISpinBox*)
			getElementFromId(E_DIALOG_ID_SCREEN_HEIGHT, true);
		u32 height = spin->getValue();
		conf->set("screen_height", std::to_string(height));

		u32 flags = 0;
		if (isBoxChecked(E_DIALOG_ID_DEBUG_BBOX))
			flags |= EDS_BBOX;
		if (isBoxChecked(E_DIALOG_ID_DEBUG_NORMALS))
			flags |= EDS_NORMALS;
		if (isBoxChecked(E_DIALOG_ID_DEBUG_SKELETON))
			flags |= EDS_SKELETON;
		if (isBoxChecked(E_DIALOG_ID_DEBUG_WIREFRANE))
			flags |= EDS_MESH_WIRE_OVERLAY;
		if (isBoxChecked(E_DIALOG_ID_DEBUG_ALPHA))
			flags |= EDS_HALF_TRANSPARENCY;
		if (isBoxChecked(E_DIALOG_ID_DEBUG_BUFFERS))
			flags |= EDS_BBOX_BUFFERS;
		conf->set("debug_flags", std::to_string(flags));

		flags = 0;
		if (isBoxChecked(E_DIALOG_ID_EXPORT_ANIM))
			flags |= E_MESH_EXPORT_ANIM;
		if (isBoxChecked(E_DIALOG_ID_EXPORT_TRANSFORM))
			flags |= E_MESH_EXPORT_TRANSFORM;
		if (isBoxChecked(E_DIALOG_ID_EXPORT_FLIP))
			flags |= E_MESH_EXPORT_FLIP;
		if (isBoxChecked(E_DIALOG_ID_EXPORT_NORMAL))
			flags |= E_MESH_EXPORT_NORMAL;
		conf->set("export_flags", std::to_string(flags));

		spin = (IGUISpinBox*)
			getElementFromId(E_DIALOG_ID_EXPORT_SCALE, true);
		u32 scale = spin->getValue();
		conf->set("export_scale", std::to_string(scale));
	}
	return IGUIElement::OnEvent(event);
}

TexturesDialog::TexturesDialog(IGUIEnvironment *env, IGUIElement *parent,
	s32 id, const rect<s32> &rectangle, Config *conf, ISceneManager *smgr) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
	conf(conf),
	smgr(smgr)
{
	IGUITabControl *tabs = env->addTabControl(rect<s32>(2,2,398,280), this,
		true, true);

	IGUITab *tab_model = tabs->addTab(L"Model");
	IGUITab *tab_wield = tabs->addTab(L"Wield");
	IGUIEditBox *edit;
	IGUIButton *button;
	IGUICheckBox *check;
	stringw fn;
	std::string key;

	bool model_texture_single = conf->getBool("model_texture_single");
	bool wield_texture_single = conf->getBool("wield_texture_single");
	check = env->addCheckBox(false, rect<s32>(15,20,380,40), tab_model,
		E_DIALOG_ID_TEXTURES_1_MODEL, L"Use single texture for all layers");
	check->setChecked(model_texture_single);
	check = env->addCheckBox(false, rect<s32>(15,20,380,40), tab_wield,
		E_DIALOG_ID_TEXTURES_1_WIELD, L"Use single texture for all layers");
	check->setChecked(wield_texture_single);

	ITexture *image = getTexture("browse.png");
	ISceneNode *model = smgr->getSceneNodeFromId(E_SCENE_ID_MODEL);
	ISceneNode *wield = smgr->getSceneNodeFromId(E_SCENE_ID_WIELD);
	u32 mc_model = 0;
	if (model)
		mc_model = (model_texture_single) ? 1 : model->getMaterialCount();
	u32 mc_wield = 0;
	if (wield)
		mc_wield = (wield_texture_single) ? 1 : wield->getMaterialCount();

	for (u32 i = 0; i < 6; ++i)
	{
		s32 top = i * 30 + 50;
		stringw num = stringw(i + 1);

		key = "model_texture_" + std::to_string(i + 1);
		fn = conf->getCStr(key);
		env->addStaticText(num.c_str(), rect<s32>(15,top,25,top+20),
			false, false, tab_model, -1);
		edit = env->addEditBox(fn.c_str(), rect<s32>(35,top,350,top+20),
			true, tab_model, E_TEXTURE_ID_MODEL + i);
		edit->setEnabled(i < mc_model);
		edit->setOverrideColor(SColor(255,255,0,0));
		edit->enableOverrideColor(false);
		button = env->addButton(rect<s32>(360,top,380,top+20), tab_model,
			E_BUTTON_ID_MODEL + i);
		button->setToolTipText(L"Browse");
		if (image)
		{
			button->setImage(image);
			button->setUseAlphaChannel(true);
			button->setDrawBorder(false);
		}
		button->setEnabled(i < mc_model);

		key = "wield_texture_" + std::to_string(i + 1);
		fn = conf->getCStr(key);
		env->addStaticText(num.c_str(), rect<s32>(15,top,25,top+20),
			false, false, tab_wield, -1);
		edit = env->addEditBox(fn.c_str(), rect<s32>(35,top,350,top+20),
			true, tab_wield, E_TEXTURE_ID_WIELD + i);
		edit->setEnabled(i < mc_wield);
		edit->setOverrideColor(SColor(255,255,0,0));
		edit->enableOverrideColor(false);
		button = env->addButton(rect<s32>(360,top,380,top+20), tab_wield,
			E_BUTTON_ID_WIELD + i);
		if (image)
		{
			button->setImage(image);
			button->setUseAlphaChannel(true);
			button->setDrawBorder(false);
		}
		button->setEnabled(i < mc_wield);
	}
	button = env->addButton(rect<s32>(315,285,395,315), this,
		E_DIALOG_ID_TEXTURES_OK, L"OK");
	button = env->addButton(rect<s32>(230,285,310,315), this,
		E_DIALOG_ID_TEXTURES_CANCEL, L"Cancel");
}

ITexture *TexturesDialog::getTexture(const io::path &filename)
{
	IVideoDriver *driver = Environment->getVideoDriver();
	ITexture *texture = driver->findTexture(filename);
	if (!texture)
		texture = driver->getTexture(filename);
	return texture;
}

bool TexturesDialog::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			if (event.GUIEvent.Caller->getType() == EGUIET_EDIT_BOX)
			{
				IGUIEditBox *edit = (IGUIEditBox*)event.GUIEvent.Caller;
				if (edit)
				{
					stringc fn = stringc(edit->getText()).c_str();
					edit->enableOverrideColor(!(getTexture(fn)));
				}
			}
		}
		else if (event.GUIEvent.EventType == EGET_CHECKBOX_CHANGED)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUICheckBox *check = (IGUICheckBox*)getElementFromId(id, true);
			std::string is_checked = (check->isChecked()) ? "true" : "false";

			ISceneNode *node = 0;
			s32 edit_box_id;
			if (id == E_DIALOG_ID_TEXTURES_1_MODEL)
			{
				node = smgr->getSceneNodeFromId(E_SCENE_ID_MODEL);
				edit_box_id = E_TEXTURE_ID_MODEL;
				conf->set("model_texture_single", is_checked);
			}
			else if (id == E_DIALOG_ID_TEXTURES_1_WIELD)
			{
				node = smgr->getSceneNodeFromId(E_SCENE_ID_WIELD);
				edit_box_id = E_TEXTURE_ID_WIELD;
				conf->set("wield_texture_single", is_checked);
			}
			if (node)
			{
				for (u32 i = 1; i < 6; ++i)
				{
					if (i < node->getMaterialCount())
					{
						IGUIEditBox *edit = (IGUIEditBox*)
							getElementFromId(edit_box_id + i, true);
						edit->setEnabled(is_checked == "false");
					}
				}
			}
		}
		else if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			s32 id = event.GUIEvent.Caller->getID();
			if (id == E_DIALOG_ID_TEXTURES_OK)
			{
				IGUIEditBox *edit;
				ITexture *texture = 0;
				stringc fn;
				for (u32 i = 0; i < 6; ++i)
				{
					std::string idx = std::to_string(i + 1);
					edit = (IGUIEditBox*)
						getElementFromId(E_TEXTURE_ID_MODEL + i, true);

					fn = stringc(edit->getText()).c_str();
					texture = getTexture(fn);
					if (texture)
					{
						std::string key = "model_texture_" + idx;
						conf->set(key, fn.c_str());
					}
					edit = (IGUIEditBox*)
						getElementFromId(E_TEXTURE_ID_WIELD + i, true);

					fn = stringc(edit->getText()).c_str();
					texture = getTexture(fn);
					if (texture)
					{
						std::string key = "wield_texture_" + idx;
						conf->set(key, fn.c_str());
					}
				}
			}
			else
			{
				IGUIEditBox *edit = 0;
				s32 edit_id = 0;
				for (s32 i = 0; i < 6; ++i)
				{
					if (id == E_BUTTON_ID_MODEL + i)
						edit_id = E_TEXTURE_ID_MODEL + i;

					else if (id == E_BUTTON_ID_WIELD + i)
						edit_id = E_TEXTURE_ID_WIELD + i;

					if (edit_id)
					{
						edit = (IGUIEditBox*)getElementFromId(edit_id, true);
						break;
					}
				}
				if (edit)
				{
					const char *fn = dialog::fileOpenDialog(
						Environment->getFileSystem(), "Open Image File",
						dialog::texture_filters, dialog::texture_filter_count);
					if (fn)
					{
						edit->setText(stringw(fn).c_str());
						edit->enableOverrideColor(!(getTexture(fn)));
					}
				}
			}
		}
	}
	return IGUIElement::OnEvent(event);
}

LightsDialog::LightsDialog(IGUIEnvironment *env, IGUIElement *parent,
	s32 id, const rect<s32> &rectangle, Config *conf, ISceneManager *smgr) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
	conf(conf),
	smgr(smgr)
{
	IGUITabControl *tabs = env->addTabControl(rect<s32>(2,2,398,310),
		this, true, true);
	for (int i = 0; i < 3; ++i)
	{
		stringw label = "Light ";
		label.append(stringw(i + 1));
		IGUITab *tab_light = tabs->addTab(label.c_str());
		IGUISpinBox *spin;
		ColorCtrl *color;
		VectorCtrl *position;

		env->addStaticText(L"Type:", rect<s32>(20,20,80,40), false, false,
			tab_light);
		IGUIComboBox *combo = env->addComboBox(rect<s32>(80,20,200,40),
			tab_light, E_DIALOG_ID_LIGHT_TYPE + i);
		combo->addItem(L"Point", ELT_POINT);
		combo->addItem(L"Spot", ELT_SPOT);
		combo->addItem(L"Directional", ELT_DIRECTIONAL);
		env->addStaticText(L"Radius:", rect<s32>(240,20,300,40), false, false,
			tab_light, -1);
		spin = env->addSpinBox(L"", rect<s32>(300,20,370,40),
			true, tab_light, E_DIALOG_ID_LIGHT_RADIUS + i);
		spin->setDecimalPlaces(0);
		color = new ColorCtrl(env, tab_light, E_DIALOG_ID_LIGHT_DIFFUSE + i,
			rect<s32>(20,50,270,70), L"Diffuse Color:");
		color->drop();
		color = new ColorCtrl(env, tab_light, E_DIALOG_ID_LIGHT_AMBIENT + i,
			rect<s32>(20,80,270,100), L"Ambient Color:");
		color->drop();
		color = new ColorCtrl(env, tab_light, E_DIALOG_ID_LIGHT_SPECULAR + i,
			rect<s32>(20,110,270,130), L"Specular Color:");
		color->drop();
		position = new VectorCtrl(env, tab_light, E_DIALOG_ID_LIGHT_POS + i,
			rect<s32>(10,140,150,250), 1.0, L"Position:");
		position->drop();
		position = new VectorCtrl(env, tab_light, E_DIALOG_ID_LIGHT_ROT + i,
			rect<s32>(240,140,380,250), 1.0, L"Rotation:");
		position->drop();
	}
	env->addButton(rect<s32>(5,315,85,345), this,
		E_DIALOG_ID_LIGHTS_PREVIEW, L"Preview");
	env->addButton(rect<s32>(315,315,395,345), this,
		E_DIALOG_ID_LIGHTS_OK, L"OK");
	env->addButton(rect<s32>(230,315,310,345), this,
		E_DIALOG_ID_LIGHTS_CANCEL, L"Cancel");

	setLights(E_DIALOG_ID_LIGHTS_INIT);
}

void LightsDialog::setLights(s32 id)
{
	for (int i = 0; i < 3; ++i)
	{
		std::string n = std::to_string(i + 1);
		IGUIComboBox *combo = (IGUIComboBox*)
			getElementFromId(E_DIALOG_ID_LIGHT_TYPE + i, true);
		s32 index = combo->getSelected();
		E_LIGHT_TYPE light_type = (E_LIGHT_TYPE)combo->getItemData(index);
		ColorCtrl *diffuse = (ColorCtrl*)
			getElementFromId(E_DIALOG_ID_LIGHT_DIFFUSE + i, true);
		ColorCtrl *ambient = (ColorCtrl*)
			getElementFromId(E_DIALOG_ID_LIGHT_AMBIENT + i, true);
		ColorCtrl *specular = (ColorCtrl*)
			getElementFromId(E_DIALOG_ID_LIGHT_SPECULAR + i, true);
		IGUISpinBox *radius = (IGUISpinBox*)
			getElementFromId(E_DIALOG_ID_LIGHT_RADIUS + i, true);
		VectorCtrl *position = (VectorCtrl*)
			getElementFromId(E_DIALOG_ID_LIGHT_POS + i, true);
		VectorCtrl *rotation = (VectorCtrl*)
			getElementFromId(E_DIALOG_ID_LIGHT_ROT + i, true);
		if (id == E_DIALOG_ID_LIGHTS_OK)
		{
			conf->set("light_type_" + n, std::to_string(light_type));
			conf->set("light_color_diffuse_" + n, diffuse->getString());
			conf->set("light_color_ambient_" + n, ambient->getString());
			conf->set("light_color_specular_" + n, specular->getString());
			conf->set("light_position_" + n, position->getString());
			conf->set("light_rotation_" + n, rotation->getString());
			conf->set("light_radius_" + n, std::to_string(radius->getValue()));
		}
		else if (id != E_DIALOG_ID_LIGHTS_PREVIEW)
		{
			light_type = (E_LIGHT_TYPE)conf->getInt("light_type_" + n);
			index = combo->getIndexForItemData(light_type);
			combo->setSelected(index);
			diffuse->setColor(conf->get("light_color_diffuse_" + n));
			ambient->setColor(conf->get("light_color_ambient_" + n));
			specular->setColor(conf->get("light_color_specular_" + n));
			Vector pos = conf->getVector("light_position_" + n);
			position->setVector(vector3df(pos.x, pos.y, pos.z));
			Vector rot = conf->getVector("light_rotation_" + n);
			rotation->setVector(vector3df(rot.x, rot.y, rot.z));
			radius->setValue(conf->getInt("light_radius_" + n));
		}
		LightSource *light = (LightSource*)
			smgr->getSceneNodeFromId(E_SCENE_ID_LIGHT + i);
		if (light)
		{
			LightSpec lightspec;
			lightspec.type = light_type;
			lightspec.position = position->getVector();
			lightspec.rotation = rotation->getVector();
			lightspec.radius = radius->getValue();
			lightspec.color.diffuse = diffuse->getColor();
			lightspec.color.ambient = ambient->getColor();
			lightspec.color.specular = specular->getColor();
			light->setLight(lightspec);
		}
	}
}

bool LightsDialog::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			s32 id = event.GUIEvent.Caller->getID();
			if (id == E_DIALOG_ID_LIGHTS_OK ||
				id == E_DIALOG_ID_LIGHTS_CANCEL ||
				id == E_DIALOG_ID_LIGHTS_PREVIEW)
			{
				setLights(id);
			}
		}
	}
	return IGUIElement::OnEvent(event);
}