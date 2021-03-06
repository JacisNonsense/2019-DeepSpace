#pragma once

#include "StateDevice.h"
#include "SensoredTransmission.h"
#include "sensors/BinarySensor.h"

namespace curtinfrc {
  struct ElevatorConfig {
    SensoredTransmission &spool;
    double spoolRadius; // In metres    // Plus average thickness of rope wrapped around spool...?
    sensors::BinarySensor *limitSensorTop;
    sensors::BinarySensor *limitSensorBottom;
  };

  enum ElevatorState { kStationary, kMoving, kZeroing, kManual };
  class Elevator : public StateDevice<ElevatorState> {
   public:
    Elevator(ElevatorConfig config) : _config(config) {};

    void SetManual(double setpoint);
    void SetSetpoint(double setpoint);
    void SetZeroing();
    void SetHold();

    double GetSetpoint();

    ElevatorConfig &GetConfig();
    
   protected:
    virtual void OnStateChange(ElevatorState newState, ElevatorState oldState) override {};
    virtual void OnStatePeriodic(ElevatorState state, double dt) override;

   private:
    ElevatorConfig _config;
    double _setpoint;
  };
} // ns curtinfrc
