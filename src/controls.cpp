#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
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

void ColorCtrl::setColor(const SColor &color)
{
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(2) << std::hex << color.getRed();
	ss << std::setfill('0') << std::setw(2) << std::hex << color.getGreen();
	ss << std::setfill('0') << std::setw(2) << std::hex << color.getBlue();
	std::string hex = ss.str();
	for (auto &val : hex)
	{
		val = toupper(val);
	}
	setColor(hex);
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
	else if (event.EventType == EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
	{
		IGUIElement *elem = getElementFromPoint(
			vector2di(event.MouseInput.X, event.MouseInput.Y));
		if (elem && elem->getType() == EGUIET_IMAGE)
		{
			vector2di p = elem->getAbsolutePosition().UpperLeftCorner;
			IGUIWindow *window = Environment->addWindow(
				rect<s32>(p.X+40, p.Y, p.X+320, p.Y+270), true, L"Color");
			ColorChooser *dialog = new ColorChooser(Environment, window, this,
				E_CTRL_ID_COLOR_CHOOSER, rect<s32>(0,20,280,270), getColor());
			dialog->drop();
		}
	}
	return IGUIElement::OnEvent(event);
}

ColorChooser::ColorChooser(IGUIEnvironment *env, IGUIElement *parent,
		ColorCtrl *receiver, s32 id, const rect<s32> &rectangle,
		SColor color_orig) :
	IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
	receiver(receiver),
	color_orig(color_orig),
	color_selected(color_orig)
{
	IGUISpinBox *spin;
	env->addStaticText(L"R", rect<s32>(180,20,200,40),
		false, false, this);
	spin = env->addSpinBox(L"", rect<s32>(200,20,260,40),
		true, this, E_CTRL_ID_COLOR_RED);
	spin->setDecimalPlaces(0);
	spin->setRange(0, 255);
	env->addStaticText(L"G", rect<s32>(180,50,200,70),
		false, false, this);
	spin = env->addSpinBox(L"", rect<s32>(200,50,260,70),
		true, this, E_CTRL_ID_COLOR_GREEN);
	spin->setDecimalPlaces(0);
	spin->setRange(0, 255);
	env->addStaticText(L"B", rect<s32>(180,80,200,100),
		false, false, this);
	spin = env->addSpinBox(L"", rect<s32>(200,80,260,100),
		true, this, E_CTRL_ID_COLOR_BLUE);
	spin->setDecimalPlaces(0);
	spin->setRange(0, 255);
	env->addStaticText(L"H", rect<s32>(180,120,200,140),
		false, false, this);
	spin = env->addSpinBox(L"", rect<s32>(200,120,260,140),
		true, this, E_CTRL_ID_COLOR_HUE);
	spin->setDecimalPlaces(0);
	spin->setRange(0, 360);
	env->addStaticText(L"S", rect<s32>(180,150,200,170),
		false, false, this);
	spin = env->addSpinBox(L"", rect<s32>(200,150,260,170),
		true, this, E_CTRL_ID_COLOR_SAT);
	spin->setDecimalPlaces(0);
	spin->setRange(0, 100);
	env->addStaticText(L"L", rect<s32>(180,180,200,200),
		false, false, this);
	spin = env->addSpinBox(L"", rect<s32>(200,180,260,200),
		true, this, E_CTRL_ID_COLOR_LUM);
	spin->setDecimalPlaces(0);
	spin->setValue(0);
	spin->setRange(0, 100);

	env->addButton(rect<s32>(195,215,275,245), this,
		E_CTRL_ID_COLOR_OK, L"OK");
	env->addButton(rect<s32>(110,215,190,245), this,
		E_CTRL_ID_COLOR_CANCEL, L"Cancel");

	color_hsl.fromRGB(color_orig);
	setColor();
}

SColor ColorChooser::getPixelColor(const vector2di &pos)
{
	IVideoDriver *driver = Environment->getVideoDriver();
	IImage *capture = driver->createScreenShot(ECF_R8G8B8);
	SColor color = capture->getPixel(pos.X, pos.Y);
	capture->drop();
	return color;
}

void ColorChooser::setColor()
{
	IGUISpinBox *spin;
	spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_RED);
	spin->setValue(color_selected.getRed());
	spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_GREEN);
	spin->setValue(color_selected.getGreen());
	spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_BLUE);
	spin->setValue(color_selected.getBlue());

	spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_HUE);
	spin->setValue(color_hsl.Hue);
	spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_SAT);
	spin->setValue(color_hsl.Saturation);
	spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_LUM);
	spin->setValue(color_hsl.Luminance);
}

void ColorChooser::draw()
{
	IVideoDriver *driver = Environment->getVideoDriver();
	vector2di p = getAbsolutePosition().UpperLeftCorner;
	rect_color = rect<s32>(p.X+20,p.Y+20,p.X+40,p.Y+200);
	rect_shade = rect<s32>(p.X+50,p.Y+20,p.X+169,p.Y+199);
	rect_orig = rect<s32>(p.X+10,p.Y+215,p.X+50,p.Y+240);
	for (s32 i = 0; i < 180; ++i)
	{
		SColorHSL hsl(i * 2, 100.f, 50.f);
		SColorf color;
		hsl.toRGB(color);
		driver->draw2DLine(vector2d<s32>(p.X+20,p.Y+20+i),
			vector2d<s32>(p.X+40,p.Y+20+i), color.toSColor());
	}
	SColorf color;
	SColorHSL hsl = color_hsl;
	hsl.Saturation = 100.f;
	hsl.Luminance = 50.f;
	hsl.toRGB(color);
	driver->draw2DRectangle(rect<s32>(p.X+50,p.Y+20,p.X+170,p.Y+200),
		color.toSColor(), color.toSColor(), SColor(255,0,0,0),
		SColor(255,255,255,255));
	driver->draw2DRectangle(color_orig, rect_orig);
	driver->draw2DRectangle(color_selected,
		rect<s32>(p.X+60,p.Y+215,p.X+100,p.Y+240));
	IGUIElement::draw();
}

bool ColorChooser::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		s32 id = event.GUIEvent.Caller->getID();
		if (event.GUIEvent.EventType == EGET_SPINBOX_CHANGED)
		{
			IGUISpinBox *spin = 0;
			switch (id)
			{
			case E_CTRL_ID_COLOR_RED:
			case E_CTRL_ID_COLOR_GREEN:
			case E_CTRL_ID_COLOR_BLUE:
			{
				spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_RED);
				color_selected.setRed((u32)spin->getValue());
				spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_GREEN);
				color_selected.setGreen((u32)spin->getValue());
				spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_BLUE);
				color_selected.setBlue((u32)spin->getValue());
				color_hsl.fromRGB(color_selected);
				break;
			}
			case E_CTRL_ID_COLOR_HUE:
			case E_CTRL_ID_COLOR_SAT:
			case E_CTRL_ID_COLOR_LUM:
			{
				spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_HUE);
				color_hsl.Hue = spin->getValue();
				spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_SAT);
				color_hsl.Saturation = spin->getValue();
				spin = (IGUISpinBox*)getElementFromId(E_CTRL_ID_COLOR_LUM);
				color_hsl.Luminance = spin->getValue();
				SColorf color;
				color_hsl.toRGB(color);
				color_selected = color.toSColor();
				break;
			}
			default:
				break;
			}
			setColor();
		}
		else if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			if (id = E_CTRL_ID_COLOR_OK)
				 receiver->setColor(color_selected);
			getParent()->remove();
		}
	}
	else if (event.EventType == EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
	{
		vector2di pos = vector2di(event.MouseInput.X, event.MouseInput.Y);
		if (rect_color.isPointInside(pos) || rect_orig.isPointInside(pos))
		{
			color_selected = getPixelColor(pos);
			color_hsl.fromRGB(color_selected);
			setColor();
		}
		else if (rect_shade.isPointInside(pos))
		{
			// ignore hue component of shade gradient
			SColorf color;
			SColor rgb = getPixelColor(pos);
			SColorHSL hsl;
			hsl.fromRGB(rgb);
			color_hsl.Saturation = hsl.Saturation;
			color_hsl.Luminance = hsl.Luminance;
			color_hsl.toRGB(color);
			color_selected = color.toSColor();
			setColor();
		}
		return true;
	}
	return IGUIElement::OnEvent(event);
}
