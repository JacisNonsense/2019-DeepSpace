#include "simulation/windows/control.h"

#include "mockdata/DriverStationData.h"

#include <csignal>
#include <iostream>

using namespace simulation;

control_window::control_window(std::vector<std::unique_ptr<ui::window>> &all_windows_ref)
    : ui::window("Simulation Control", 250, 400), _all_windows_ref(all_windows_ref) {

  exit.on_activate([&](bool, ui::button &) { do_exit(); });
  
  respawn.set_can_activate(false);
  respawn.on_click([&](bool, ui::button &) {
    std::for_each(all_windows_ref.begin(), all_windows_ref.end(), [](std::unique_ptr<ui::window> &window) {
      if (!window->is_running())
        window->start();
    });
  });

  register_button(exit);
  register_button(respawn);

  double mode_y = 0.3;
  double mode_height = 0.1;
  ui::colour mode_deactive{0.2, 0.2, 0.2};

  mode_buttons.emplace_back(ui::box{0, mode_y, 1, mode_height}, "DISABLED", mode_deactive, ui::colour{0.5, 0.25, 0.25},
                            ui::colour{1, 0.5, 0.5});
  mode_buttons.back().on_activate([&](bool active, ui::button &me) {
    toggle_modes(active, me);
    HALSIM_SetDriverStationEnabled(!active);
  });

  mode_y += mode_height;

  mode_buttons.emplace_back(ui::box{0, mode_y, 1, mode_height}, "AUTO", mode_deactive, ui::colour{0.25, 0.25, 0.5},
                            ui::colour{0.5, 0.5, 1});
  mode_buttons.back().on_activate([&](bool active, ui::button &me) {
    toggle_modes(active, me);
    HALSIM_SetDriverStationAutonomous(active);
  });

  mode_y += mode_height;

  mode_buttons.emplace_back(ui::box{0, mode_y, 1, mode_height}, "TELEOP", mode_deactive, ui::colour{0.25, 0.5, 0.25},
                            ui::colour{0.5, 1, 0.5});
  mode_buttons.back().on_activate([&](bool active, ui::button &me) { toggle_modes(active, me); });

  mode_y += mode_height;

  mode_buttons.emplace_back(ui::box{0, mode_y, 1, mode_height}, "TEST", mode_deactive, ui::colour{0.5, 0.25, 0.5},
                            ui::colour{1, 0.5, 1});
  mode_buttons.back().on_activate([&](bool active, ui::button &me) {
    toggle_modes(active, me);
    HALSIM_SetDriverStationTest(active);
  });

  mode_y += mode_height;

  for (auto it = mode_buttons.begin(); it != mode_buttons.end(); it++) {
    register_button(*it);
  }
}

#include "simulation/joystick_controller.h"

void control_window::render(cv::Mat &img) {
  ui::point{0.5, 0.05}.text(img, "Sim Controls", 1.0, ui::colour::white(), 2);
  ui::point{0.5, 0.35}.text(img, "Mode", 1.0, ui::colour::white(), 2);


  ui::point{0.5, 0.8}.text(img, (std::to_string(joystick_controller::which_button(0))).c_str(), 1.0, ui::colour::white(), 2);
  int active_axis = joystick_controller::which_axis(0);
  int axis_value = joystick_controller::axis_raw(0, active_axis);
  ui::point{0.5, 0.9}.text(img, (std::to_string(active_axis) + " = " + std::to_string(axis_value)).c_str(), 1.0, ui::colour::white(), 2);
}

void control_window::on_start() {
  mode_buttons[0].activate();  // Enter disabled mode
}

void control_window::on_stop() {
  mode_buttons[0].activate();  // Enter disabled mode
}

void control_window::on_closed() {
  do_exit();
}

void control_window::do_exit() {
  std::cout << "[SIMCTRL] SIGTERM requested" << std::endl;
  raise(SIGTERM);
}

void control_window::toggle_modes(bool active, ui::button &me) {
  if (active) {
    for (auto it = mode_buttons.begin(); it != mode_buttons.end(); it++) {
      if (it->id != me.id) it->deactivate();
    }
  }
}