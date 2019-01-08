#pragma once

namespace simulation {

struct dialect_entry {
  enum class type {
    UNASSIGNED, BUTTON, AXIS, DISCARD
  };
  type component = type::UNASSIGNED;
  int index = -1;
};

struct joystick_dialect {
  dialect_entry buttons[32];
  dialect_entry axis[10];

  static const joystick_dialect ps4();
  static const joystick_dialect xbox360();
};

class joystick_controller {
 public:
  joystick_controller() = delete;

  static void start();
  static void rescan();
  static void update();

  static int count();

  static bool available(int id);

  // BUTTONS //
  static int    num_buttons(int id);
  static bool   button(int id, int button);
  static bool   any_button(int id);
  static int    which_button(int id);

  // AXIS //
  static int    num_axis(int id);
  static int    axis_raw(int id, int axis);
  static float  axis(int id, int axis);
  static int    which_axis(int id);

  // POV //
  static int    num_pov(int id);
  static int    pov(int id, int pov);
};

}  // namespace simulation