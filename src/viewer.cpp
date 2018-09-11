#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <irrlicht.h>

#include "config.h"
#include "scene.h"
#include "trackball.h"
#include "gui.h"
#include "dialog.h"
#include "viewer.h"

#define M_ZOOM_IN(fov) std::max(fov - DEGTORAD * 2, PI * 0.0125f)
#define M_ZOOM_OUT(fov) std::min(fov + DEGTORAD * 2, PI * 0.5f)

Viewer::Viewer(Config *conf) :
	conf(conf),
	device(0),
	camera(0),
	scene(0),
	trackball(0),
	gui(0),
	animation(0)
{}

Viewer::~Viewer()
{
	if (scene)
		scene->drop();
	if (trackball)
		delete trackball;
	if (gui)
		delete gui;
	if (animation)
		delete animation;
}

bool Viewer::run(IrrlichtDevice *irr_device)
{
	device = irr_device;
	device->getFileSystem()->addFileArchive("../assets/");
	device->getFileSystem()->addFileArchive("../media/");
	device->getFileSystem()->changeWorkingDirectoryTo("../media/");
	device->setEventReceiver(this);

	IVideoDriver *driver = device->getVideoDriver();
	ISceneManager *smgr = device->getSceneManager();
	IGUIEnvironment *env = device->getGUIEnvironment();

	screen = driver->getScreenSize();
	trackball = new Trackball(screen.Width, screen.Height);
	scene = new Scene(smgr->getRootSceneNode(), smgr, E_SCENE_ID);
	scene->addAnimator(trackball);

	gui = new GUI(device, conf);
	gui->initMenu();
	gui->initToolBar();

	if (!scene->load(conf))
		return false;

	animation = new AnimState(env);
	animation->load(scene->getNode(E_SCENE_ID_MODEL));
	animation->setField(E_GUI_ID_ANIM_START, conf->getInt("anim_start"));
	animation->setField(E_GUI_ID_ANIM_END, conf->getInt("anim_end"));
	animation->setField(E_GUI_ID_ANIM_SPEED, conf->getInt("anim_speed"));
	animation->setField(E_GUI_ID_ANIM_FRAME, conf->getInt("anim_start"));
	scene->setAnimation(conf->getInt("anim_start"),
		conf->getInt("anim_start"), conf->getInt("anim_speed"));

	camera = smgr->addCameraSceneNode(0, vector3df(0,0,30), vector3df(0,0,0));
	fov = camera->getFOV();
	fov_home = fov;
	jump_time = 0;

	setCaptionFileName(conf->getCStr("model_mesh"));
	setBackgroundColor(conf->getHex("bg_color"));
	setProjection();

	while (device->run())
	{
		resize();
		if (dialog::has_event)
			dialog::has_event = !OnEvent(dialog::event);
		driver->beginScene(true, true, bg_color);
		smgr->drawAll();
		env->drawAll();
		driver->endScene();
		animation->update(scene->getNode(E_SCENE_ID_MODEL));
	}
	return true;
}

void Viewer::resize()
{
	IVideoDriver *driver = device->getVideoDriver();
	dimension2du dim = driver->getScreenSize();
	if (screen == dim)
		return;

	const vector2di move = vector2di(dim.Width - screen.Width, 0);
	gui->moveElement(E_GUI_ID_TOOLBOX_MODEL, move);
	gui->moveElement(E_GUI_ID_ANIM_CTRL, move);

	screen = dim;
	trackball->setBounds(screen.Width, screen.Height);
	camera->setAspectRatio((f32)screen.Width / (f32)screen.Height);
	setProjection();
}

void Viewer::setProjection()
{
	f32 width = (f32)screen.Width * fov / 20.0f;
	f32 height = (f32)screen.Height * fov / 20.0f;
	ortho.buildProjectionMatrixOrthoLH(width, height, 1.0f, 1000.f);

	if (conf->getBool("ortho"))
		camera->setProjectionMatrix(ortho, true);
	else
		camera->setFOV(fov);
}

void Viewer::setBackgroundColor(const u32 &color)
{
	bg_color.color = color;
	bg_color.setAlpha(255);
}

void Viewer::setCaptionFileName(const io::path &filename)
{
	io::IFileSystem *fs = device->getFileSystem();
	stringw caption = fs->getFileBasename(filename) + L" - SAM-Viewer";
	device->setWindowCaption(caption.c_str());
}

static inline std::string boolToString(bool b)
{
	return (b) ? "true" : "false";
}

static inline std::string vectorToString(vector3df v)
{
	std::ostringstream ss;
	ss << v.X << "," << v.Y << "," << v.Z;
	std::string str(ss.str());
	return str;
}

bool Viewer::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		if (event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED)
		{
			IGUIContextMenu *menu = (IGUIContextMenu*)event.GUIEvent.Caller;
			s32 item = menu->getSelectedItem();
			s32 id = menu->getItemCommandId(item);
			IGUIEnvironment *env = device->getGUIEnvironment();

			switch (id)
			{
			case E_GUI_ID_LOAD_MODEL_MESH:
				dialog::showFileOpen(env, E_GUI_ID_LOAD_MODEL_MESH,
					"Open main model file", dialog::model_filters,
					dialog::model_filter_count);
				break;
			case E_GUI_ID_LOAD_WIELD_MESH:
				dialog::showFileOpen(env, E_GUI_ID_LOAD_WIELD_MESH,
					"Open wield model file", dialog::model_filters,
					dialog::model_filter_count);
				break;
			case E_GUI_ID_ENABLE_WIELD:
			{
				ISceneNode *wield = scene->getNode(E_SCENE_ID_WIELD);
				if (wield)
				{
					wield->setVisible(menu->isItemChecked(item));
					conf->set("wield_show",
						boolToString(menu->isItemChecked(item)));
				}
				break;
			}
			case E_GUI_ID_QUIT:
				device->closeDevice();
				break;
			case E_GUI_ID_TEXTURES_DIALOG:
				gui->showTexturesDialog();
				break;
			case E_GUI_ID_SETTINGS_DIALOG:
				gui->showSettingsDialog();
				break;
			case E_GUI_ID_TOOLBOX_MODEL:
			{
				if (menu->isItemChecked(item))
					gui->showToolBox(E_GUI_ID_TOOLBOX_MODEL);
				else
					gui->closeToolBox(E_GUI_ID_TOOLBOX_MODEL);
				break;
			}
			case E_GUI_ID_TOOLBOX_WIELD:
			{
				if (menu->isItemChecked(item))
					gui->showToolBox(E_GUI_ID_TOOLBOX_WIELD);
				else
					gui->closeToolBox(E_GUI_ID_TOOLBOX_WIELD);
				break;
			}
			case E_GUI_ID_SHOW_GRID:
				scene->setGridVisible(menu->isItemChecked(item));
				menu->setItemEnabled(item + 1, menu->isItemChecked(item));
				break;
			case E_GUI_ID_SHOW_AXES:
				scene->setAxesVisible(menu->isItemChecked(item));
				break;
			case E_GUI_ID_BILINEAR:
				scene->setFilter(EMF_BILINEAR_FILTER,
					menu->isItemChecked(item));
					conf->set("bilinear",
						boolToString(menu->isItemChecked(item)));
				break;
			case E_GUI_ID_TRILINEAR:
				scene->setFilter(EMF_TRILINEAR_FILTER,
					menu->isItemChecked(item));
					conf->set("trilinear",
						boolToString(menu->isItemChecked(item)));
				break;
			case E_GUI_ID_ANISOTROPIC:
				scene->setFilter(EMF_ANISOTROPIC_FILTER,
					menu->isItemChecked(item));
					conf->set("anisotropic",
						boolToString(menu->isItemChecked(item)));
				break;
			case E_GUI_ID_PERSPECTIVE:
				menu->setItemChecked(item + 1, !menu->isItemChecked(item));
				conf->set("ortho", boolToString(!menu->isItemChecked(item)));
				setProjection();
				break;
			case E_GUI_ID_ORTHOGONAL:
				menu->setItemChecked(item - 1, !menu->isItemChecked(item));
				conf->set("ortho", boolToString(menu->isItemChecked(item)));
				setProjection();
				break;
			case E_GUI_ID_BACK_FACE_CULL:
				scene->setBackFaceCulling(menu->isItemChecked(item));
				conf->set("backface_cull",
					boolToString(menu->isItemChecked(item)));
				break;
			case E_GUI_ID_DEBUG_INFO:
				scene->setDebugInfo(menu->isItemChecked(item));
				conf->set("debug_info",
					boolToString(menu->isItemChecked(item)));
				break;
			case E_DIALOG_ID_ABOUT:
				gui->showAboutDialog();
				break;
			case E_GUI_ID_SAVE_CONFIG:
			{
				ISceneNode *model = scene->getNode(E_SCENE_ID_MODEL);
				if (model)
				{
					conf->set("model_position",
						vectorToString(model->getPosition()));
					conf->set("model_rotation",
						vectorToString(model->getRotation()));
					conf->set("model_scale",
						std::to_string(model->getScale().Y * 100));
					conf->set("model_material",
						std::to_string(model->getMaterial(0).MaterialType));
				}
				ISceneNode *wield = scene->getNode(E_SCENE_ID_WIELD);
				if (wield)
				{
					conf->set("wield_position",
						vectorToString(wield->getPosition()));
					conf->set("wield_rotation",
						vectorToString(wield->getRotation()));
					conf->set("wield_scale",
						std::to_string(wield->getScale().Y * 100));
					conf->set("wield_material",
						std::to_string(wield->getMaterial(0).MaterialType));
				}
				conf->set("anim_start",
					std::to_string(animation->getField(E_GUI_ID_ANIM_START)));
				conf->set("anim_end",
					std::to_string(animation->getField(E_GUI_ID_ANIM_END)));
				conf->set("anim_speed",
					std::to_string(animation->getField(E_GUI_ID_ANIM_SPEED)));
				conf->save();
				break;
			}
			default:
				break;
			}
		}
		else if (event.GUIEvent.EventType == EGET_FILE_SELECTED)
		{
			stringc fn = (dialog::filename) ? dialog::filename : "";
			s32 id = event.UserEvent.UserData1;
			gui->setFocused(false);
			switch (id)
			{
			case E_GUI_ID_LOAD_MODEL_MESH:
			{
				if (!fn.empty() && scene->loadModelMesh(fn))
				{
					ISceneNode *model = scene->getNode(E_SCENE_ID_MODEL);
					if (model)
					{
						animation->load(model);
						setCaptionFileName(fn);
						gui->reloadToolBox(E_GUI_ID_TOOLBOX_MODEL);
						conf->set("model_mesh", fn.c_str());
					}
				}
				return true;
			}
			case E_GUI_ID_LOAD_WIELD_MESH:
			{
				if (!fn.empty() && scene->loadWieldMesh(fn))
				{
					gui->reloadToolBox(E_GUI_ID_TOOLBOX_WIELD);
					conf->set("wield_mesh", fn.c_str());
				}
				return true;
			}
			default:
				break;
			}
		}
		else if (event.GUIEvent.EventType == EGET_FILE_CHOOSE_DIALOG_CANCELLED)
		{
			gui->setFocused(false);
		}
		else if (event.GUIEvent.EventType == EGET_ELEMENT_CLOSED)
		{
			IGUIContextMenu *menu =
				(IGUIContextMenu*)gui->getElement(E_GUI_ID_MENU);
			if (menu)
			{
				s32 id = event.GUIEvent.Caller->getID();
				if (id == E_GUI_ID_TOOLBOX_MODEL)
					menu->getSubMenu(2)->setItemChecked(0, false);
				else if (id == E_GUI_ID_TOOLBOX_WIELD)
					menu->getSubMenu(2)->setItemChecked(1, false);
			}
			gui->setFocused(false);
		}
		else if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			s32 id = event.GUIEvent.Caller->getID();
			switch (id)
			{
			case E_GUI_ID_SKIP_REV:
				scene->setAnimation(
					animation->getField(E_GUI_ID_ANIM_START),
					animation->getField(E_GUI_ID_ANIM_START),
					animation->getField(E_GUI_ID_ANIM_SPEED));
				animation->setState(E_ANIM_STATE_PAUSED);
				gui->setFocused(false);
				break;
			case E_GUI_ID_PLAY_REV:
				scene->setAnimation(
					animation->getField(E_GUI_ID_ANIM_START),
					animation->getField(E_GUI_ID_ANIM_END),
					-animation->getField(E_GUI_ID_ANIM_SPEED));
				animation->setState(E_ANIM_STATE_PLAY_REV);
				gui->setFocused(false);
				break;
			case E_GUI_ID_PAUSE:
				scene->setAnimation(
					animation->getFrame(),
					animation->getFrame(),
					animation->getField(E_GUI_ID_ANIM_SPEED));
				animation->setState(E_ANIM_STATE_PAUSED);
				gui->setFocused(false);
				break;
			case E_GUI_ID_PLAY_FWD:
				scene->setAnimation(
					animation->getField(E_GUI_ID_ANIM_START),
					animation->getField(E_GUI_ID_ANIM_END),
					animation->getField(E_GUI_ID_ANIM_SPEED));
				animation->setState(E_ANIM_STATE_PLAY_FWD);
				gui->setFocused(false);
				break;
			case E_GUI_ID_SKIP_FWD:
				scene->setAnimation(
					animation->getField(E_GUI_ID_ANIM_END),
					animation->getField(E_GUI_ID_ANIM_END),
					animation->getField(E_GUI_ID_ANIM_SPEED));
				animation->setState(E_ANIM_STATE_PAUSED);
				gui->setFocused(false);
				break;
			case E_DIALOG_ID_SETTINGS_OK:
				event.GUIEvent.Caller->getParent()->getParent()->remove();
				setBackgroundColor(conf->getHex("bg_color"));
				scene->setGridColor(conf->getHex("grid_color"));
				scene->setAttachment();
				scene->setDebugInfo(conf->getBool("debug_info"));
				gui->setFocused(false);
				break;
			case E_DIALOG_ID_SETTINGS_CANCEL:
			case E_DIALOG_ID_TEXTURES_CANCEL:
			case E_DIALOG_ID_ABOUT_OK:
				event.GUIEvent.Caller->getParent()->getParent()->remove();
				gui->setFocused(false);
				break;
			case E_DIALOG_ID_TEXTURES_OK:
				scene->refresh();
				event.GUIEvent.Caller->getParent()->getParent()->remove();
				gui->setFocused(false);
				break;
			default:
				break;
			}
		}
		else if (event.GUIEvent.EventType == EGET_SPINBOX_CHANGED)
		{
			s32 id = event.GUIEvent.Caller->getID();
			switch (id)
			{
			case E_GUI_ID_ANIM_START:
			case E_GUI_ID_ANIM_END:
			{
				if (animation->getState() != E_ANIM_STATE_PAUSED)
				{
					IAnimatedMeshSceneNode *model = (IAnimatedMeshSceneNode*)
						scene->getNode(E_SCENE_ID_MODEL);
					scene->setAnimation(
						animation->getField(E_GUI_ID_ANIM_START),
						animation->getField(E_GUI_ID_ANIM_END),
						model->getAnimationSpeed());
				}
				break;
			}
			case E_GUI_ID_ANIM_SPEED:
			{
				IAnimatedMeshSceneNode *model = (IAnimatedMeshSceneNode*)
					scene->getNode(E_SCENE_ID_MODEL);
				s32 speed = animation->getField(E_GUI_ID_ANIM_SPEED);
				if (animation->getState() == E_ANIM_STATE_PLAY_REV)
					speed = -speed;

				model->setAnimationSpeed(speed);
				break;
			}
			case E_GUI_ID_ANIM_FRAME:
			{
				if (animation->getState() == E_ANIM_STATE_PAUSED)
				{
					u32 frame = animation->getField(E_GUI_ID_ANIM_FRAME);
					scene->setAnimation(frame, frame,
						animation->getField(E_GUI_ID_ANIM_SPEED));
				}
				break;
			}
			default:
				break;
			}
		}
		else if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			gui->setFocused(false);
		}
		else if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUSED)
		{
			gui->setFocused(true);
			trackball->release();
		}
	}
	else if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.PressedDown && !gui->getFocused())
	{
		switch (event.KeyInput.Key)
		{
		case KEY_HOME:
		{
			scene->setRotation(vector3df(0,0,0));
			fov = fov_home;
			setProjection();
			break;
		}
		case KEY_SPACE:
		{
			u32 now_time = device->getTimer()->getTime() + 1000;
			if (jump_time + 800 < now_time)
			{
				jump_time = now_time;
				scene->jump();
			}
			break;
		}
		case KEY_LEFT:
			scene->rotate(E_SCENE_AXIS_Y, 15);
			break;
		case KEY_RIGHT:
			scene->rotate(E_SCENE_AXIS_Y, -15);
			break;
		case KEY_UP:
			scene->rotate(E_SCENE_AXIS_X, -15);
			break;
		case KEY_DOWN:
			scene->rotate(E_SCENE_AXIS_X, 15);
			break;
		case KEY_KEY_Z:
			scene->rotate(E_SCENE_AXIS_Z, -15);
			break;
		case KEY_KEY_X:
			scene->rotate(E_SCENE_AXIS_Z, 15);
			break;
		case KEY_PLUS:
			fov = M_ZOOM_IN(fov);
			setProjection();
			break;
		case KEY_MINUS:
			fov = M_ZOOM_OUT(fov);
			setProjection();
			break;
		case KEY_F5:
			scene->refresh();
			break;
		default:
			break;
		}
	}
	else if (event.EventType == EET_MOUSE_INPUT_EVENT && !gui->getFocused())
	{
		switch (event.MouseInput.Event)
		{
		case EMIE_LMOUSE_LEFT_UP:
			trackball->release();
			break;
		case EMIE_MOUSE_MOVED:
		{
			if (event.MouseInput.isLeftPressed() && !trackball->isClicked())
				trackball->click();

			trackball->setDragPos(event.MouseInput.X, event.MouseInput.Y);
			break;

		}
		case EMIE_MOUSE_WHEEL:
		{
			if (event.MouseInput.Wheel < 0)
				fov = M_ZOOM_OUT(fov);
			else
				fov = M_ZOOM_IN(fov);
			setProjection();
			return true;
		}
		default:
			break;
		}
	}
	return false;
}

AnimState::AnimState(IGUIEnvironment *env) :
	env(env),
	frame(0),
	state(E_ANIM_STATE_PAUSED)
{}

void AnimState::load(ISceneNode *node)
{
	IAnimatedMeshSceneNode *model =	(IAnimatedMeshSceneNode*)node;
	if (!model)
		return;

	s32 max = model->getEndFrame();
	bool enabled = (max > 0);
	state = E_ANIM_STATE_PAUSED;
	model->setFrameLoop(0, 0);

	initField(E_GUI_ID_ANIM_START, max, enabled);
	initField(E_GUI_ID_ANIM_END, max, enabled);
	initField(E_GUI_ID_ANIM_FRAME, max, enabled);
	initField(E_GUI_ID_ANIM_SPEED, 60, enabled);

	setField(E_GUI_ID_ANIM_START, 0);
	setField(E_GUI_ID_ANIM_END, max);
	setField(E_GUI_ID_ANIM_FRAME, 0);

	IGUIElement *root = env->getRootGUIElement();
	AnimCtrl *anim = (AnimCtrl*)
		root->getElementFromId(E_GUI_ID_ANIM_CTRL, true);
	anim->reset(enabled);
	if (enabled)
	{
		IGUIButton *button =
			(IGUIButton*)anim->getElementFromId(E_GUI_ID_PAUSE, true);
		button->setPressed(true);
	}
}

void AnimState::update(ISceneNode *node)
{
	IAnimatedMeshSceneNode *model =	(IAnimatedMeshSceneNode*)node;
	frame = model->getFrameNr();
	setField(E_GUI_ID_ANIM_FRAME, frame);
}

void AnimState::initField(s32 id, const u32 &max, const bool &enabled)
{
	IGUIElement *root = env->getRootGUIElement();
	IGUISpinBox *spin = (IGUISpinBox*)root->getElementFromId(id, true);
	spin->setRange(0, max);
	spin->setEnabled(enabled);
}

u32 AnimState::getField(s32 id)
{
	IGUIElement *root = env->getRootGUIElement();
	IGUISpinBox *spin = (IGUISpinBox*)root->getElementFromId(id, true);
	return spin->getValue();
}

void AnimState::setField(s32 id, const u32 &value)
{
	IGUIElement *root = env->getRootGUIElement();
	IGUISpinBox *spin = (IGUISpinBox*)root->getElementFromId(id, true);
	spin->setValue(value);
}
