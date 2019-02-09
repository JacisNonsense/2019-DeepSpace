#pragma once

#include <frc/Encoder.h>

namespace curtinfrc {
namespace sensors {
  class Encoder {
   public:
    Encoder(int encoderTicksPerRotation) : _encoderTicksPerRotation(encoderTicksPerRotation){};
    virtual int     GetEncoderTicks() = 0;
    virtual double  GetEncoderTickVelocity() = 0;  // ticks/s

    double GetEncoderRotations();
    int    GetEncoderTicksPerRotation();
    double GetEncoderAngularVelocity();   // rad/s
    void   ZeroEncoder();

   private:
    int _encoderTicksPerRotation;
    int _offset = 0;
  };

  class DigitalEncoder : public Encoder {
   public:
    DigitalEncoder(int channelA, int channelB, int ticksPerRotation)
        : _channelA(channelA),
          _channelB(channelB),
          _nativeEncoder(channelA, channelB),
          Encoder(ticksPerRotation){};

    int GetEncoderTicks() override;
    double GetEncoderTickVelocity() override;

    int GetChannelA();
    int GetChannelB();

   private:
    int          _channelA, _channelB;
    frc::Encoder _nativeEncoder;
  };
}  // namespace sensors
}  // namespace curtinfrc
