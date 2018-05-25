#include <stdlib.h>
#include <iostream>
#include <irrlicht.h>

#include "config.h"
#include "scene.h"
#include "dialog.h"
#include "gui.h"

VertexCtrl::VertexCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
	const rect<s32> &rectangle, f32 step, const wchar_t *label) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
	vertex(0)
{
	IGUIStaticText *text = env->addStaticText(label,
		rect<s32>(0,0,20,20), false, false, this);

	IGUISpinBox *spin = env->addSpinBox(L"", rect<s32>(20,0,120,20),
		true, this, E_GUI_ID_VERTEX);
	spin->setDecimalPlaces(2);
	spin->setValue(0);
	spin->setStepSize(step);
	spin->setRange(-1000, 1000);
}

void VertexCtrl::setValue(const f32 &value)
{
	IGUISpinBox *spin = (IGUISpinBox*)getElementFromId(E_GUI_ID_VERTEX);
	if (spin)
	{
		spin->setValue(value);
		vertex = value;
	}
}

bool VertexCtrl::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		if (event.GUIEvent.EventType == EGET_SPINBOX_CHANGED)
		{
			IGUISpinBox *spin = (IGUISpinBox*)event.GUIEvent.Caller;
			if (spin)
				vertex = spin->getValue();
		}
	}
	else if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
	{
		setValue(vertex);
	}
	return IGUIElement::OnEvent(event);
}

VectorCtrl::VectorCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
	const rect<s32> &rectangle, f32 step, const wchar_t *label) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
	vector(vector3df(0,0,0))
{
	IGUIStaticText *text = env->addStaticText(label,
		rect<s32>(10,0,150,20), false, false, this);

	VertexCtrl *x = new VertexCtrl(env, this, E_GUI_ID_VECTOR_X,
		rect<s32>(10,30,150,50), step, L"X");
	x->drop();
	VertexCtrl *y = new VertexCtrl(env, this, E_GUI_ID_VECTOR_Y,
		rect<s32>(10,60,150,80), step, L"Y");
	y->drop();
	VertexCtrl *z = new VertexCtrl(env, this, E_GUI_ID_VECTOR_Z,
		rect<s32>(10,90,150,110), step, L"Z");
	z->drop();
}

void VectorCtrl::setVector(const vector3df &vec)
{
	vector = vec;
	VertexCtrl *vertex;
	vertex = (VertexCtrl*)getElementFromId(E_GUI_ID_VECTOR_X);
	if (vertex)
		vertex->setValue(vector.X);
	vertex = (VertexCtrl*)getElementFromId(E_GUI_ID_VECTOR_Y);
	if (vertex)
		vertex->setValue(vector.Y);
	vertex = (VertexCtrl*)getElementFromId(E_GUI_ID_VECTOR_Z);
	if (vertex)
		vertex->setValue(vector.Z);
}

bool VectorCtrl::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT &&
		event.GUIEvent.EventType == EGET_SPINBOX_CHANGED)
	{
		VertexCtrl *vertex = (VertexCtrl*)event.GUIEvent.Caller->getParent();
		if (vertex)
		{
			switch (vertex->getID())
			{
			case E_GUI_ID_VECTOR_X:
				vector.X = vertex->getValue();
				break;
			case E_GUI_ID_VECTOR_Y:
				vector.Y = vertex->getValue();
				break;
			case E_GUI_ID_VECTOR_Z:
				vector.Z = vertex->getValue();
				break;
			default:
				break;
			}
			SEvent new_event = event;
			new_event.GUIEvent.Caller = this;
			return IGUIElement::OnEvent(new_event);
		}
	}
	return IGUIElement::OnEvent(event);
}

AnimCtrl::AnimCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
	const rect<s32> &rectangle) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
	button_id(E_GUI_ID_PAUSE)
{
	IVideoDriver *driver = env->getVideoDriver();
	ITexture *image;
	IGUIButton *button;

	image = driver->getTexture("skip_rev.png");
	button = env->addButton(rect<s32>(0,4,23,27), this,
		E_GUI_ID_SKIP_REV);
	button->setImage(image);
	button->setUseAlphaChannel(true);

	image = driver->getTexture("play_rev.png");
	button = env->addButton(rect<s32>(24,4,47,27), this,
		E_GUI_ID_PLAY_REV);
	button->setImage(image);
	button->setUseAlphaChannel(true);

	image = driver->getTexture("pause.png");
	button = env->addButton(rect<s32>(48,4,71,27), this,
		E_GUI_ID_PAUSE);
	button->setImage(image);
	button->setUseAlphaChannel(true);

	image = driver->getTexture("play_fwd.png");
	button = env->addButton(rect<s32>(72,4,95,27), this,
		E_GUI_ID_PLAY_FWD);
	button->setImage(image);
	button->setUseAlphaChannel(true);

	image = driver->getTexture("skip_fwd.png");
	button = env->addButton(rect<s32>(96,4,119,27), this,
		E_GUI_ID_SKIP_FWD);
	button->setImage(image);
	button->setUseAlphaChannel(true);
}

bool AnimCtrl::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			reset(true);
			IGUIButton *button = (IGUIButton*)event.GUIEvent.Caller;
			button->setEnabled(false);
			button->setPressed(true);
			button_id = button->getID();
		}
		else if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			if (button_id == E_GUI_ID_SKIP_FWD ||
				button_id == E_GUI_ID_SKIP_REV)
			{
				reset(true);
				IGUIButton *button = (IGUIButton*)
					getElementFromId(E_GUI_ID_PAUSE);
				if (button)
					button->setPressed(true);
				button_id = E_GUI_ID_PAUSE;
			}
		}
	}
	return IGUIElement::OnEvent(event);
}

void AnimCtrl::reset(bool enabled)
{
	const list<IGUIElement*> &children = getChildren();
	list<IGUIElement*>::ConstIterator iter = children.begin();
	while (iter != children.end())
	{
		if ((*iter)->getType() == EGUIET_BUTTON)
		{
			IGUIButton *button = (IGUIButton*)(*iter);
			button->setEnabled(enabled);
			button->setPressed(false);
		}
		++iter;
	}
}

ToolBox::ToolBox(IGUIEnvironment *env, IGUIElement *parent, s32 id,
		const rect<s32> &rectangle, ISceneNode *node) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle)
{
	smgr = node->getSceneManager();
	node_id = node->getID();

	IGUIWindow *dialog = (IGUIWindow*)parent;
	dialog->setDraggable(false);
	VectorCtrl *position = new VectorCtrl(env, this, E_GUI_ID_POSITION,
		rect<s32>(10,0,150,120), 1.0, L"Position:");

	VectorCtrl *rotation = new VectorCtrl(env, this, E_GUI_ID_ROTATION,
		rect<s32>(10,130,150,250), 15.0, L"Rotation:");

	IGUIStaticText *text;
	text = env->addStaticText(L"Scale:",
		rect<s32>(20,260,150,280), false, false, this);

	text = env->addStaticText(L"%",
		rect<s32>(20,290,40,310), false, false, this);

	IGUISpinBox *scale = env->addSpinBox(L"", rect<s32>(40,290,140,310),
		true, this, E_GUI_ID_SCALE);

	scale->setDecimalPlaces(0);
	scale->setStepSize(10.0);
	scale->setRange(0, 1000);

	position->setVector(node->getPosition());
	position->drop();
	rotation->setVector(node->getRotation());
	rotation->drop();
	scale->setValue(node->getScale().Y * 100);

	text = env->addStaticText(L"Transparency:",
		rect<s32>(20,330,150,350), false, false, this);

	IGUIComboBox *combo = env->addComboBox(rect<s32>(20,360,140,380),
		this, E_GUI_ID_MATERIAL);
	combo->addItem(L"Opaque");
	combo->addItem(L"Alpha Channel");
	combo->addItem(L"Alpha Test");

	switch (node->getMaterial(0).MaterialType)
	{
	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		combo->setSelected(1);
		break;
	case EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
		combo->setSelected(2);
		break;
	default:
		break;
	}
}

bool ToolBox::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		s32 id = event.GUIEvent.Caller->getID();
		if (event.GUIEvent.EventType == EGET_SPINBOX_CHANGED)
		{
			ISceneNode *node = smgr->getSceneNodeFromId(node_id);
			if (node)
			{
				IGUIElement *elem = (IGUIElement*)event.GUIEvent.Caller;
				switch(id)
				{
				case E_GUI_ID_POSITION:
				{
					VectorCtrl *position = (VectorCtrl*)elem;
					node->setPosition(position->getVector());
					break;
				}
				case E_GUI_ID_ROTATION:
				{
					VectorCtrl *rotation = (VectorCtrl*)elem;
					node->setRotation(rotation->getVector());
					break;
				}
				case E_GUI_ID_SCALE:
				{
					IGUISpinBox *spin = (IGUISpinBox*)elem;
					f32 s = spin->getValue() / 100;
					node->setScale(vector3df(s,s,s));
					break;
				}
				default:
					break;
				}
			}
		}
		else if(event.GUIEvent.EventType == EGET_COMBO_BOX_CHANGED)
		{
			ISceneNode *node = smgr->getSceneNodeFromId(node_id);
			if (node)
			{
				IGUIComboBox *combo = (IGUIComboBox*)event.GUIEvent.Caller;
				switch (combo->getSelected())
				{
				case 0:
					node->setMaterialType(EMT_SOLID);
					break;
				case 1:
					node->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
					break;
				case 2:
					node->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
					break;
				}
			}
		}
	}
	return IGUIElement::OnEvent(event);
}

GUI::GUI(IrrlichtDevice *device, Config *config) :
	device(device),
	conf(config),
	has_focus(false)
{
	IGUIEnvironment *env = device->getGUIEnvironment();
	IGUISkin *skin = env->getSkin();
	IGUIFont *font = env->getFont("fontlucida.png");
	if (font)
		skin->setFont(font);

	skin->setColor(EGDC_3D_FACE, SColor(255,232,232,232));
	skin->setColor(EGDC_3D_DARK_SHADOW, SColor(255,160,160,160));
	skin->setColor(EGDC_3D_HIGH_LIGHT, SColor(255,248,248,248));
	skin->setColor(EGDC_3D_LIGHT, SColor(255,255,255,255));
	skin->setColor(EGDC_3D_SHADOW, SColor(255,196,196,196));
	skin->setColor(EGDC_ACTIVE_BORDER, SColor(255,232,232,232));
	skin->setColor(EGDC_INACTIVE_BORDER, SColor(255,232,232,232));
	skin->setColor(EGDC_GRAY_EDITABLE, SColor(255,172,172,172));
	skin->setColor(EGDC_GRAY_TEXT, SColor(255,96,96,96));
	skin->setColor(EGDC_ACTIVE_CAPTION, SColor(255,16,16,16));
	skin->setColor(EGDC_INACTIVE_CAPTION, SColor(255,64,64,64));

	for (s32 i=0; i < EGDC_COUNT; ++i)
	{
		SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(255);
		skin->setColor((EGUI_DEFAULT_COLOR)i, col);
	}
}

void GUI::initMenu()
{
	IGUIEnvironment *env = device->getGUIEnvironment();
	IGUIContextMenu *menu = env->addMenu(0, E_GUI_ID_MENU);
	menu->addItem(L"File", -1, true, true);
	menu->addItem(L"Edit", -1, true, true);
	menu->addItem(L"View", -1, true, true);
	menu->addItem(L"Help", -1, true, true);

	IGUIContextMenu *submenu;
	submenu = menu->getSubMenu(0);
	submenu->addItem(L"Load Model Mesh", E_GUI_ID_LOAD_MODEL_MESH);
	submenu->addItem(L"Load Wield Mesh", E_GUI_ID_LOAD_WIELD_MESH);
	submenu->addSeparator();
	submenu->addItem(L"Save Configuration", E_GUI_ID_SAVE_CONFIG);
	submenu->addSeparator();
	submenu->addItem(L"Quit", E_GUI_ID_QUIT);

	submenu = menu->getSubMenu(1);
	submenu->addItem(L"Textures", E_GUI_ID_TEXTURES_DIALOG);
	submenu->addSeparator();
	submenu->addItem(L"Preferences", E_GUI_ID_SETTINGS_DIALOG);

	submenu = menu->getSubMenu(2);
	submenu->addItem(L"Model Toolbox", E_GUI_ID_TOOLBOX_MODEL, true, false,
		false, true);
	submenu->addItem(L"Wield Toolbox", E_GUI_ID_TOOLBOX_WIELD, true, false,
		false, true);
	submenu->addSeparator();
	submenu->addItem(L"Show Grid", E_GUI_ID_SHOW_GRID, true, false,
		true, true);
	submenu->addItem(L"Show Axes", E_GUI_ID_SHOW_AXES, true, false,
		true, true);
	submenu->addSeparator();
	submenu->addItem(L"Projection", -1, true, true);
	submenu->addItem(L"Filters", -1, true, true);
	submenu->addSeparator();
	submenu->addItem(L"Show Wield Item", E_GUI_ID_ENABLE_WIELD, true, false,
		conf->getBool("wield_show"), true);
	submenu->addItem(L"Backface Culling", E_GUI_ID_BACK_FACE_CULL, true, false,
		conf->getBool("backface_cull"), true);
	submenu->addSeparator();
	submenu->addItem(L"Model Debug Info", E_GUI_ID_DEBUG_INFO, true, false,
		conf->getBool("debug_info"), true);

	submenu = menu->getSubMenu(2)->getSubMenu(6);
	submenu->addItem(L"Perspective", E_GUI_ID_PERSPECTIVE, true, false,
		!conf->getBool("ortho"), true);
	submenu->addItem(L"Orthogonal", E_GUI_ID_ORTHOGONAL, true, false,
		conf->getBool("ortho"), true);

	submenu = menu->getSubMenu(2)->getSubMenu(7);
	submenu->addItem(L"Bilinear", E_GUI_ID_BILINEAR, true, false,
		conf->getBool("bilinear"), true);
	submenu->addItem(L"Trilinear", E_GUI_ID_TRILINEAR, true, false,
		conf->getBool("trilinear"), true);
	submenu->addItem(L"Anisotropic", E_GUI_ID_ANISOTROPIC, true, false,
		conf->getBool("anisotropic"), true);

	submenu = menu->getSubMenu(3);
	submenu->addItem(L"About", E_DIALOG_ID_ABOUT);
}

void GUI::initToolBar()
{
	IVideoDriver *driver = device->getVideoDriver();
	IGUIEnvironment *env = device->getGUIEnvironment();
	IGUIStaticText *text;
	IGUISpinBox *spin;

	IGUIToolBar *toolbar = env->addToolBar(0, E_GUI_ID_TOOLBAR);
	text = env->addStaticText(L"Animation:",
		rect<s32>(20,6,120,25), false, false, toolbar);

	text = env->addStaticText(L"Start",
		rect<s32>(130,6,160,25), false, false, toolbar);
	spin = env->addSpinBox(L"", rect<s32>(170,5,230,25),
		true, toolbar, E_GUI_ID_ANIM_START);
	spin->setDecimalPlaces(0);
	spin->setRange(0, 10000);

	text = env->addStaticText(L"End",
		rect<s32>(255,6,280,25), false, false, toolbar);
	spin = env->addSpinBox(L"", rect<s32>(290,5,350,25),
		true, toolbar, E_GUI_ID_ANIM_END);
	spin->setDecimalPlaces(0);
	spin->setRange(0, 10000);

	text = env->addStaticText(L"Speed",
		rect<s32>(370,6,410,25), false, false, toolbar);
	spin = env->addSpinBox(L"", rect<s32>(420,5,480,25),
		true, toolbar, E_GUI_ID_ANIM_SPEED);
	spin->setDecimalPlaces(0);
	spin->setRange(0, 10000);

	text = env->addStaticText(L"Frame",
		rect<s32>(495,6,535,25), false, false, toolbar);
	spin = env->addSpinBox(L"", rect<s32>(550,5,610,25),
		true, toolbar, E_GUI_ID_ANIM_FRAME);
	spin->setDecimalPlaces(0);
	spin->setRange(0, 10000);

	s32 w = driver->getScreenSize().Width;
	AnimCtrl *anim = new AnimCtrl(env, toolbar, E_GUI_ID_ANIM_CTRL,
		rect<s32>(w-120,0,w,30));
	anim->drop();
}

void GUI::showToolBox(s32 id)
{
	ISceneManager *smgr = device->getSceneManager();
	IGUIEnvironment *env = device->getGUIEnvironment();
	IGUIElement *root = env->getRootGUIElement();
	if (root->getElementFromId(id, true))
		return;

	switch (id)
	{
	case E_GUI_ID_TOOLBOX_MODEL:
	{
		IVideoDriver *driver = device->getVideoDriver();
		s32 w = driver->getScreenSize().Width;
		ISceneNode *node = smgr->getSceneNodeFromId(E_SCENE_ID_MODEL);
		IGUIWindow *window = env->addWindow(rect<s32>(w-160,54,w,490),
			false, L"Model Properties", root, id);
		ToolBox *toolbox = new ToolBox(env, window, id,
			rect<s32>(0,30,160,800), node);
		toolbox->drop();
		env->setFocus(window);
		break;
	}
	case E_GUI_ID_TOOLBOX_WIELD:
	{
		ISceneNode *node = smgr->getSceneNodeFromId(E_SCENE_ID_WIELD);
		IGUIWindow *window = env->addWindow(rect<s32>(0,54,160,490),
			false, L"Wield Properties", root, id);
		ToolBox *toolbox = new ToolBox(env, window, id,
			rect<s32>(0,30,160,800), node);
		toolbox->drop();
		env->setFocus(window);
		break;
	}
	default:
		break;
	}
}

void GUI::closeToolBox(s32 id)
{
	IGUIElement *elem = getElement(id);
	if (elem)
		elem->remove();
}

void GUI::reloadToolBox(s32 id)
{
	IGUIElement *elem = getElement(id);
	if (elem)
	{
		elem->remove();
		showToolBox(id);
	}
}

void GUI::moveElement(s32 id, const vector2di &move)
{
	IGUIElement *elem = getElement(id);
	if (elem)
	{
		vector2di pos = elem->getRelativePosition().UpperLeftCorner + move;
		elem->setRelativePosition(position2di(pos.X, pos.Y));
	}
}

void GUI::showTexturesDialog()
{
	IGUIEnvironment *env = device->getGUIEnvironment();
	ISceneManager *smgr = device->getSceneManager();
	IGUIWindow *window = env->addWindow(getWindowRect(400, 310),
		true, L"Textures");

	TexturesDialog *dialog = new TexturesDialog(env, window,
		E_DIALOG_ID_TEXTURES, rect<s32>(0,20,400,310), conf, smgr);
	dialog->drop();
}

void GUI::showSettingsDialog()
{
	IGUIEnvironment *env = device->getGUIEnvironment();
	IGUIWindow *window = env->addWindow(getWindowRect(400, 310),
		true, L"Settings");

	SettingsDialog *dialog = new SettingsDialog(env, window,
		E_DIALOG_ID_SETTINGS, rect<s32>(0,20,400,310), conf);
	dialog->drop();
}

void GUI::showAboutDialog()
{
	IGUIEnvironment *env = device->getGUIEnvironment();
	IGUIWindow *window = env->addWindow(getWindowRect(300, 300),
		true, L"About");

	AboutDialog *dialog = new AboutDialog(env, window,
		E_DIALOG_ID_ABOUT, rect<s32>(0,20,300,300));
	dialog->drop();
}

IGUIElement *GUI::getElement(s32 id)
{
	IGUIEnvironment *env = device->getGUIEnvironment();
	IGUIElement *root = env->getRootGUIElement();
	return root->getElementFromId(id, true);
}

const rect<s32> GUI::getWindowRect(const u32 &width, const u32 &height) const
{
	IVideoDriver *driver = device->getVideoDriver();
	dimension2du s = driver->getScreenSize();
	vector2di pos = vector2di((s.Width - width) / 2, (s.Height - height) / 2);
	return rect<s32>(pos, pos + vector2di(width, height));
}