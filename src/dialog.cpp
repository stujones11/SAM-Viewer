#include <stdlib.h>
#include <iostream>
#include <irrlicht.h>

#include "config.h"
#include "scene.h"
#include "dialog.h"

#ifdef USE_CMAKE_CONFIG_H
#include "cmake_config.h"
#else
#define D_ABOUT_LINK_URL "https://github.com/stujones11/SAM-Viewer"
#define D_ABOUT_LINK_TEXT "github.com/stujones11/SAM-Viewer"
#define D_VERSION "0.0.0"
#endif

static inline void open_url(std::string url)
{
	system((std::string("xdg-open \"") + url + std::string("\"")).c_str());
}

HyperlinkCtrl::HyperlinkCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
	const rect<s32> &rectangle, std::string title, std::string url) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
	url(url),
	is_active(false)
{
	IGUIStaticText *text = env->addStaticText(stringw(title.c_str()).c_str(),
		rect<s32>(0,0,rectangle.getWidth(),20), false, false, this);
	text->setOverrideColor(SColor(255,0,0,255));
	text->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
}

void HyperlinkCtrl::draw()
{
	if (is_active)
	{
		IVideoDriver *driver = Environment->getVideoDriver();
		rect<s32> pos = getAbsolutePosition();
		vector2di end = pos.LowerRightCorner;
		vector2di start = end - vector2di(pos.getWidth(), 0);
		driver->draw2DLine(start, end, SColor(255,0,0,255));
	}
	IGUIElement::draw();
}

bool HyperlinkCtrl::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		if (event.GUIEvent.EventType == EGET_ELEMENT_HOVERED)
			is_active = true;
		else if (event.GUIEvent.EventType == EGET_ELEMENT_LEFT)
			is_active = false;
	}
	else if (is_active && event.EventType == EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
	{
		open_url(url);
	}
	return IGUIElement::OnEvent(event);
}

static inline bool isValidHexString(std::string hex)
{
	return (hex.length() == 6 &&
		hex.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos);
}

ColorCtrl::ColorCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
	const rect<s32> &rectangle, const wchar_t *label) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle)
{
	IVideoDriver *driver = env->getVideoDriver();
	IGUIStaticText *text = env->addStaticText(label, rect<s32>(0,0,160,20),
		false, false, this);
	IGUIEditBox *edit = env->addEditBox(L"", rect<s32>(180,0,250,20), true,
		this, E_DIALOG_ID_COLOR_EDIT);
	edit->setMax(6);
	edit->setToolTipText(L"Hex color RRGGBB");

	ITexture *texture = driver->findTexture("color_preview");
	if (!texture)
	{
		IImage *image = driver->createImage(ECF_A8R8G8B8, dimension2du(30,20));
		image->fill(SColor(255,255,255,255));
		texture = driver->addTexture("color_preview", image);
		image->drop();
	}
	IGUIImage *preview = env->addImage(rect<s32>(270,0,300,20), this,
		E_DIALOG_ID_COLOR_PREVIEW);
	preview->setImage(texture);
}

void ColorCtrl::setColor(const std::string &hex)
{
	if (!isValidHexString(hex))
		return;

	stringw text = hex.c_str();
	IGUIEditBox *edit = (IGUIEditBox*)
		getElementFromId(E_DIALOG_ID_COLOR_EDIT);
	if (edit)
		edit->setText(text.c_str());
	IGUIImage *preview = (IGUIImage*)
		getElementFromId(E_DIALOG_ID_COLOR_PREVIEW);
	if (preview)
	{
		SColor color;
		color.color = std::stoul(hex, nullptr, 16);
		color.setAlpha(255);
		preview->setColor(color);
	}
}

std::string ColorCtrl::getColor() const
{
	std::string hex = "";
	IGUIEditBox *edit = (IGUIEditBox*)
		getElementFromId(E_DIALOG_ID_COLOR_EDIT);
	if (edit)
		hex = stringc(edit->getText()).c_str();
	return hex;
}

bool ColorCtrl::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT &&
		event.GUIEvent.EventType == EGET_EDITBOX_CHANGED &&
		event.GUIEvent.Caller->getID() == E_DIALOG_ID_COLOR_EDIT)
	{
		IGUIEditBox *edit = (IGUIEditBox*)event.GUIEvent.Caller;
		std::string hex = stringc(edit->getText()).c_str();
		setColor(hex);
	}
	return IGUIElement::OnEvent(event);
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

	IGUIButton *button = env->addButton(rect<s32>(110,235,190,265), this,
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
	IGUIStaticText *text;
	IGUIEditBox *edit;
	IGUISpinBox *spin;
	IGUICheckBox *check;
	IGUIButton *button;
	ColorCtrl *color;

	color = new ColorCtrl(env, tab_general, E_DIALOG_ID_BG_COLOR,
		rect<s32>(20,20,320,40), L"Background Color:");
	color->setColor(conf->get("bg_color"));
	color->drop();

	color = new ColorCtrl(env, tab_general, E_DIALOG_ID_GRID_COLOR,
		rect<s32>(20,50,320,70), L"Grid Color:");
	color->setColor(conf->get("grid_color"));
	color->drop();

	text = env->addStaticText(L"Wield Attachment Bone:", rect<s32>(20,80,180,100),
		false, false, tab_general, -1);
	stringw bone_name = conf->getCStr("wield_bone");
	edit = env->addEditBox(bone_name.c_str(), rect<s32>(200,80,320,100),
		true, tab_general, E_DIALOG_ID_WIELD_BONE);

	text = env->addStaticText(L"Default Screen Width:",
		rect<s32>(20,110,180,130), false, false, tab_general, -1);
	spin = env->addSpinBox(L"", rect<s32>(200,110,270,130),
		true, tab_general, E_DIALOG_ID_SCREEN_WIDTH);
	spin->setValue(conf->getInt("screen_width"));
	spin->setDecimalPlaces(0);

	text = env->addStaticText(L"Default Screen Height:",
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
		E_DIALOG_ID_DEBUG_WIREFRANE, L"Wireframe overaly");
	check->setChecked(conf->getInt("debug_flags") & EDS_MESH_WIRE_OVERLAY);
	check = env->addCheckBox(false, rect<s32>(20,140,380,160), tab_debug,
		E_DIALOG_ID_DEBUG_ALPHA, L"Use transparent material");
	check->setChecked(conf->getInt("debug_flags") & EDS_HALF_TRANSPARENCY);
	check = env->addCheckBox(false, rect<s32>(20,170,380,190),	tab_debug,
		E_DIALOG_ID_DEBUG_BUFFERS, L"Show all mesh buffers");
	check->setChecked(conf->getInt("debug_flags") & EDS_BBOX_BUFFERS);

	button = env->addButton(rect<s32>(315,255,395,285), this,
		E_DIALOG_ID_SETTINGS_OK, L"OK");
	button = env->addButton(rect<s32>(230,255,310,285), this,
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
			const std::string hex = color->getColor();
			if (isValidHexString(hex))
				conf->set("bg_color", hex);
		}
		color = (ColorCtrl*)getElementFromId(E_DIALOG_ID_GRID_COLOR, true);
		if (color)
		{
			const std::string hex = color->getColor();
			if (isValidHexString(hex))
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
		s32 id = event.GUIEvent.Caller->getID();
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
				for (s32 i = 0; i < 6; ++i)
				{
					if (id == E_BUTTON_ID_MODEL + i)
					{
						Environment->addFileOpenDialog(L"Open Image File",
							 true, this, E_TEXTURE_ID_MODEL + i);
						break;
					}
					else if (id == E_BUTTON_ID_WIELD + i)
					{
						Environment->addFileOpenDialog(L"Open Image File",
							 true, this, E_TEXTURE_ID_WIELD + i);
						break;
					}
				}
			}
		}
		else if (event.GUIEvent.EventType == EGET_FILE_SELECTED)
		{
			IGUIFileOpenDialog *dialog =
				(IGUIFileOpenDialog*)event.GUIEvent.Caller;

			stringw fn = stringw(dialog->getFileName());
			if (!fn.empty())
			{
				s32 id = dialog->getID();
				IGUIEditBox *edit = (IGUIEditBox*)getElementFromId(id, true);
				if (edit)
				{
					edit->setText(fn.c_str());
					edit->enableOverrideColor(!(getTexture(fn)));
				}
			}
		}
	}
	return IGUIElement::OnEvent(event);
}
