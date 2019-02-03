#include "control/PIDController.h"

#include <cmath>
#include <frc/smartdashboard/SendableBase.h>
#include <frc/smartdashboard/SendableBuilder.h>

#include <networktables/NetworkTableInstance.h>

#include <iostream>

using namespace curtinfrc::control;

// PIDGains

PIDGains::PIDGains(std::string name, double kP, double kI, double kD, double kF) : _name(name), _kP(kP), _kI(kI), _kD(kD), _kF(kF) {
  _table = nt::NetworkTableInstance::GetDefault().GetTable("PIDGains[" + name + "]");

  _ntbounds.emplace_back(_table, "kP", &_kP);
  _ntbounds.emplace_back(_table, "kI", &_kI);
  _ntbounds.emplace_back(_table, "kD", &_kD);
  _ntbounds.emplace_back(_table, "kF", &_kF);
}

double PIDGains::kP() const {
  return _kP;
}

double PIDGains::kI() const {
  return _kI;
}

double PIDGains::kD() const {
  return _kD;
}

double PIDGains::kF() const {
  return _kF;
}

// PIDController

PIDController::PIDController(PIDGains gains) : gains(gains) {
  _setpoint = 0;
  _integError = 0;
  _lastError = 0;
}

PIDController::PIDController(std::string name, double kP, double kI, double kD, double kF) : PIDController(PIDGains{name, kP, kI, kD, kF}) { }

void PIDController::SetSetpoint(double sp) {
  _setpoint = sp;
}

double PIDController::GetSetpoint() const {
  return _setpoint;
}

void PIDController::SetDerivativeMode(bool onProcessVariable) {
  _pvDeriv = onProcessVariable;
}

void PIDController::ResetIAccum() {
  _integError = 0;
}

void PIDController::SetIAccumZone(double errorIResetThreshold) {
  _iAccumZone = errorIResetThreshold;
}

void PIDController::SetICeiling(double ceiling) {
  _iCeiling = ceiling;
}

double PIDController::Calculate(double processVariable, double dt, double feedforward) {
  double error = _setpoint - processVariable;
  
  if (_iAccumZone > 1e-6 && std::abs(error) > _iAccumZone)
    ResetIAccum();

  _integError += error * dt;

  if (_iCeiling > 1e-6 && std::abs(_integError) > _iCeiling)
    _integError = (_integError > 0 ? 1 : -1) * _iCeiling;

  double deriv = _pvDeriv ? (processVariable - _lastPV) : (error - _lastError);

  double p = gains.kP() * error;
  double i = gains.kI() * _integError;
  double d = gains.kD() * (dt > 0 ? deriv / dt : 0);
  double f = gains.kF() * feedforward;

  _lastError = error;
  _lastPV = processVariable;
  return p + i + d + f;
}