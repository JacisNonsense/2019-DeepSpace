#include "simulation/joystick_controller.h"

#include <OIS.h>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace simulation;

static OIS::InputManager *_manager;
static std::vector<OIS::JoyStick *> _joys;

void joystick_controller::start() {
  OIS::ParamList pl;
  _manager = OIS::InputManager::createInputSystem(pl);
  update();
  rescan();
  update();
}

void joystick_controller::rescan() {
  std::cout << "Freeing " << _joys.size() << " joysticks for rescan..." << std::endl;
  for (int i = 0; i < _joys.size(); i++) {
    _manager->destroyInputObject(_joys[i]);
  }
  _joys.clear();

  int num_joy = _manager->getNumberOfDevices(OIS::Type::OISJoyStick);
  std::cout << "Found " << num_joy << " joysticks" << std::endl;
  for (int i = 0; i < num_joy; i++) {
    OIS::JoyStick *joy = (OIS::JoyStick *)_manager->createInputObject(OIS::Type::OISJoyStick, false);
    std::cout << " -> " <<  joy->vendor() << std::endl;
    _joys.push_back(joy);
  }
}

void joystick_controller::update() {
  std::for_each(_joys.begin(), _joys.end(), [](OIS::JoyStick *j) {
    j->capture();
  });
}

int joystick_controller::count() {
  return _joys.size();
}

bool joystick_controller::available(int id) {
  return id < _joys.size();
}

// BUTTONS //

int joystick_controller::num_buttons(int id) {
  return _joys[id]->getJoyStickState().mButtons.size();
}

bool joystick_controller::any_button(int id) {
  std::vector<bool> buttons = _joys[id]->getJoyStickState().mButtons;
  return std::any_of(buttons.begin(), buttons.end(), [](bool b) { return b; });
}

bool joystick_controller::button(int id, int button) {
  std::vector<bool> buttons = _joys[id]->getJoyStickState().mButtons;
  return buttons[button];
}

int joystick_controller::which_button(int id) {
  std::vector<bool> buttons = _joys[id]->getJoyStickState().mButtons;
  auto it = std::find(buttons.begin(), buttons.end(), true);
  return it == buttons.end() ? -1 : std::distance(buttons.begin(), it);
}

// AXIS //

int joystick_controller::num_axis(int id) {
  return _joys[id]->getJoyStickState().mAxes.size();
}

int joystick_controller::axis_raw(int id, int axis) {
  return _joys[id]->getJoyStickState().mAxes[axis].abs;
}

float joystick_controller::axis(int id, int axis) {
  return (float)axis_raw(id, axis) / (float)OIS::JoyStick::MAX_AXIS;
}

int joystick_controller::which_axis(int id) {
  std::vector<OIS::Axis> all_axis = _joys[id]->getJoyStickState().mAxes;
  auto it = std::find_if(all_axis.begin(), all_axis.end(), [](OIS::Axis &axis) {
    return axis.abs > (OIS::JoyStick::MAX_AXIS / 4);
  });
  return it == all_axis.end() ? -1 : std::distance(all_axis.begin(), it);
}

// POV //

int joystick_controller::num_pov(int id) {
  return 4;
}

int joystick_controller::pov(int id, int pov) {
  return _joys[id]->getJoyStickState().mPOV[pov].direction;
}

// DIALECTS

const joystick_dialect joystick_dialect::ps4() {
  joystick_dialect dialect;
#ifdef _WIN32
  // TODO: UNMAPPED
#elif __APPLE__
  dialect.buttons[1] = dialect_entry{dialect_entry::type::BUTTON, 1};   // X
  dialect.buttons[2] = dialect_entry{dialect_entry::type::BUTTON, 2};   // O
  dialect.buttons[3] = dialect_entry{dialect_entry::type::BUTTON, 3};   // T
  dialect.buttons[0] = dialect_entry{dialect_entry::type::BUTTON, 0};   // S

  dialect.buttons[4] = dialect_entry{dialect_entry::type::BUTTON, 4};   // LB
  dialect.buttons[5] = dialect_entry{dialect_entry::type::BUTTON, 5};   // RB

  dialect.buttons[6] = dialect_entry{dialect_entry::type::BUTTON, 14};  // LT BINARY
  dialect.buttons[7] = dialect_entry{dialect_entry::type::BUTTON, 15};  // RT BINARY

  dialect.buttons[8] = dialect_entry{dialect_entry::type::BUTTON, 8};   // SHARE
  dialect.buttons[9] = dialect_entry{dialect_entry::type::BUTTON, 9};   // OPTIONS
  dialect.buttons[12] = dialect_entry{dialect_entry::type::BUTTON, 12}; // PS BUTTON

  dialect.buttons[10] = dialect_entry{dialect_entry::type::BUTTON, 10}; // L STICK
  dialect.buttons[11] = dialect_entry{dialect_entry::type::BUTTON, 11}; // R STICK

  dialect.axis[0] = dialect_entry{dialect_entry::type::AXIS, 0};  // LX
  dialect.axis[1] = dialect_entry{dialect_entry::type::AXIS, 1};  // LY
  dialect.axis[2] = dialect_entry{dialect_entry::type::AXIS, 2};  // RX
  dialect.axis[3] = dialect_entry{dialect_entry::type::AXIS, 5};  // RY

  // ?
  // dialect.axis[2] = dialect_entry{dialect_entry::type::AXIS, 3};  // LT
  // dialect.axis[5] = dialect_entry{dialect_entry::type::AXIS, 4};  // RT
#elif __linux__
  dialect.buttons[0] = dialect_entry{dialect_entry::type::BUTTON, 1};   // X
  dialect.buttons[1] = dialect_entry{dialect_entry::type::BUTTON, 2};   // O
  dialect.buttons[2] = dialect_entry{dialect_entry::type::BUTTON, 3};   // T
  dialect.buttons[3] = dialect_entry{dialect_entry::type::BUTTON, 0};   // S

  dialect.buttons[4] = dialect_entry{dialect_entry::type::BUTTON, 4};   // LB
  dialect.buttons[5] = dialect_entry{dialect_entry::type::BUTTON, 5};   // RB

  dialect.buttons[6] = dialect_entry{dialect_entry::type::BUTTON, 6};   // LT BINARY
  dialect.buttons[7] = dialect_entry{dialect_entry::type::BUTTON, 7};   // RT BINARY

  dialect.buttons[8] = dialect_entry{dialect_entry::type::BUTTON, 8};   // SHARE
  dialect.buttons[9] = dialect_entry{dialect_entry::type::BUTTON, 9};   // OPTIONS
  dialect.buttons[10] = dialect_entry{dialect_entry::type::BUTTON, 12}; // PS BUTTON

  dialect.buttons[11] = dialect_entry{dialect_entry::type::BUTTON, 10}; // L STICK
  dialect.buttons[12] = dialect_entry{dialect_entry::type::BUTTON, 11}; // R STICK

  dialect.axis[0] = dialect_entry{dialect_entry::type::AXIS, 0};  // LX
  dialect.axis[1] = dialect_entry{dialect_entry::type::AXIS, 1};  // LY
  dialect.axis[3] = dialect_entry{dialect_entry::type::AXIS, 2};  // RX
  dialect.axis[4] = dialect_entry{dialect_entry::type::AXIS, 5};  // RY

  dialect.axis[2] = dialect_entry{dialect_entry::type::AXIS, 3};  // LT
  dialect.axis[5] = dialect_entry{dialect_entry::type::AXIS, 4};  // RT
#endif
  return dialect;
}