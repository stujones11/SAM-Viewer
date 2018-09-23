#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <irrlicht.h>

#include "controls.h"

VertexCtrl::VertexCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
	const rect<s32> &rectangle, f32 step, const wchar_t *label) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
	vertex(0)
{
	env->addStaticText(label, rect<s32>(0,0,20,20),
		false, false, this);

	IGUISpinBox *spin = env->addSpinBox(L"", rect<s32>(20,0,120,20),
		true, this, E_CTRL_ID_VERTEX);
	spin->setDecimalPlaces(2);
	spin->setValue(0);
	spin->setStepSize(step);
	spin->setRange(-1000, 1000);
}

void VertexCtrl::setValue(const f32 &value)
{
	IGUISpinBox *spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_VERTEX);
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
	env->addStaticText(label, rect<s32>(10,0,150,20),
		false, false, this);

	VertexCtrl *x = new VertexCtrl(env, this, E_CTRL_ID_VECTOR_X,
		rect<s32>(10,30,150,50), step, L"X");
	x->drop();
	VertexCtrl *y = new VertexCtrl(env, this, E_CTRL_ID_VECTOR_Y,
		rect<s32>(10,60,150,80), step, L"Y");
	y->drop();
	VertexCtrl *z = new VertexCtrl(env, this, E_CTRL_ID_VECTOR_Z,
		rect<s32>(10,90,150,110), step, L"Z");
	z->drop();
}

std::string VectorCtrl::getString() const
{
	std::ostringstream ss;
	ss << vector.X << "," << vector.Y << "," << vector.Z;
	std::string str(ss.str());
	return str;
}

void VectorCtrl::setVector(const vector3df &vec)
{
	vector = vec;
	VertexCtrl *vertex;
	vertex = (VertexCtrl*)getElementFromId(E_CTRL_ID_VECTOR_X);
	if (vertex)
		vertex->setValue(vector.X);
	vertex = (VertexCtrl*)getElementFromId(E_CTRL_ID_VECTOR_Y);
	if (vertex)
		vertex->setValue(vector.Y);
	vertex = (VertexCtrl*)getElementFromId(E_CTRL_ID_VECTOR_Z);
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
			case E_CTRL_ID_VECTOR_X:
				vector.X = vertex->getValue();
				break;
			case E_CTRL_ID_VECTOR_Y:
				vector.Y = vertex->getValue();
				break;
			case E_CTRL_ID_VECTOR_Z:
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
	button_id(E_CTRL_ID_PAUSE)
{
	IVideoDriver *driver = env->getVideoDriver();
	ITexture *image;
	IGUIButton *button;

	image = driver->getTexture("skip_rev.png");
	button = env->addButton(rect<s32>(0,4,23,27), this,
		E_CTRL_ID_SKIP_REV);
	button->setImage(image);
	button->setUseAlphaChannel(true);

	image = driver->getTexture("play_rev.png");
	button = env->addButton(rect<s32>(24,4,47,27), this,
		E_CTRL_ID_PLAY_REV);
	button->setImage(image);
	button->setUseAlphaChannel(true);

	image = driver->getTexture("pause.png");
	button = env->addButton(rect<s32>(48,4,71,27), this,
		E_CTRL_ID_PAUSE);
	button->setImage(image);
	button->setUseAlphaChannel(true);

	image = driver->getTexture("play_fwd.png");
	button = env->addButton(rect<s32>(72,4,95,27), this,
		E_CTRL_ID_PLAY_FWD);
	button->setImage(image);
	button->setUseAlphaChannel(true);

	image = driver->getTexture("skip_fwd.png");
	button = env->addButton(rect<s32>(96,4,119,27), this,
		E_CTRL_ID_SKIP_FWD);
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
			if (button_id == E_CTRL_ID_SKIP_FWD ||
				button_id == E_CTRL_ID_SKIP_REV)
			{
				reset(true);
				IGUIButton *button = (IGUIButton*)
					getElementFromId(E_CTRL_ID_PAUSE);
				if (button)
					button->setPressed(true);
				button_id = E_CTRL_ID_PAUSE;
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


void HyperlinkCtrl::openBrowser(std::string url)
{
	system((std::string("xdg-open \"") + url + std::string("\"")).c_str());
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
		openBrowser(url);
	}
	return IGUIElement::OnEvent(event);
}

ColorCtrl::ColorCtrl(IGUIEnvironment *env, IGUIElement *parent, s32 id,
	const rect<s32> &rectangle, const wchar_t *label) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle)
{
	IVideoDriver *driver = env->getVideoDriver();
	s32 x = rectangle.getWidth() - 140;
	env->addStaticText(label, rect<s32>(0,0,160,20), false, false, this);

	IGUIEditBox *edit = env->addEditBox(L"", rect<s32>(x,0,x+70,20), true,
		this, E_CTRL_ID_COLOR_EDIT);
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
	IGUIImage *preview = env->addImage(rect<s32>(x+90,0,x+120,20), this,
		E_CTRL_ID_COLOR_PREVIEW);
	preview->setImage(texture);
}

void ColorCtrl::setColor(const std::string &hex)
{
	if (!isValidHexString(hex))
		return;

	stringw text = hex.c_str();
	IGUIEditBox *edit = (IGUIEditBox*)
		getElementFromId(E_CTRL_ID_COLOR_EDIT);
	if (edit)
		edit->setText(text.c_str());
	IGUIImage *preview = (IGUIImage*)
		getElementFromId(E_CTRL_ID_COLOR_PREVIEW);
	if (preview)
	{
		SColor color;
		color.color = std::stoul(hex, nullptr, 16);
		color.setAlpha(255);
		preview->setColor(color);
	}
}

std::string ColorCtrl::getString() const
{
	std::string hex = "";
	IGUIEditBox *edit = (IGUIEditBox*)
		getElementFromId(E_CTRL_ID_COLOR_EDIT);
	if (edit)
		hex = stringc(edit->getText()).c_str();
	return hex;
}

SColor ColorCtrl::getColor() const
{
	IGUIImage *image = (IGUIImage*)
		getElementFromId(E_CTRL_ID_COLOR_PREVIEW);
	return image->getColor();
}

bool ColorCtrl::isValidHexString(std::string hex)
{
	return (hex.length() == 6 &&
		hex.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos);
}

bool ColorCtrl::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT &&
		event.GUIEvent.EventType == EGET_EDITBOX_CHANGED &&
		event.GUIEvent.Caller->getID() == E_CTRL_ID_COLOR_EDIT)
	{
		IGUIEditBox *edit = (IGUIEditBox*)event.GUIEvent.Caller;
		std::string hex = stringc(edit->getText()).c_str();
		setColor(hex);
	}
	return IGUIElement::OnEvent(event);
}