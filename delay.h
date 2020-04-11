// -*-c++-*-

#ifndef _DELAY_H
#define _DELAY_H

#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "fx.h"

using namespace std;

class Mustang;

class DelayCC : public FX {

protected:
  Mustang * amp;
  unsigned char model[2];
  unsigned char slot;

  int continuous_control( int parm5, int parm6, int parm7, int value, unsigned char *cmd );
  int discrete_control( int parm5, int parm6, int parm7, int value, unsigned char *cmd );



public:
  string name;
  vector<string> paramName;
  vector<int> paramCC;
  vector<int> param;
  bool enabled;

  DelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : 
    amp(theAmp), 
    slot(theSlot) 
  {
    memcpy( this->model, model, 2 );
    paramCC = {    	49, 	50, 	51, 	52, 	53, 54 };
  }

  int dispatch( int cc, int value, unsigned char *cmd );
  const unsigned char *getModel( void ) { return model;}
  const unsigned char getSlot( void ) { return slot;}

  const std::string to_json(void) {
    std::stringstream ss;
    ss   << "{ \"name\": \"" << name << "\", "
         << " \"type\": \"Delay\", "
	 << " \"enabled\": " << ( enabled ? "true": "false") << ", "
	 << " \"cc\": 25, "
         << "  \"params\": { ";
    for (auto i=0; i < paramName.size(); i++) {
        if (i) ss << ",";
        ss << " \"" << paramName[i] << "\": ";
      	ss << "[" << param[i] << "," << std::dec << paramCC[i] << "]";
        //ss << " \"" << paramName[i] << "\": " << param[i] << " ";
    }
    ss   << "}}";
    return ss.str();
  }



private:
  // Level
  virtual int cc49( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Delay Time
  virtual int cc50( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x06, value, cmd );}
  
  virtual int cc51( int value, unsigned char *cmd ) = 0;
  virtual int cc52( int value, unsigned char *cmd ) = 0;
  virtual int cc53( int value, unsigned char *cmd ) = 0;
  virtual int cc54( int value, unsigned char *cmd ) = 0;
};


class MonoDelayCC : public DelayCC {
public:
  MonoDelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : DelayCC(theAmp,model,theSlot) {
      name = "Mono Delay";
      paramName = { "Level", "Delay Time", "Feedback", "Brightness", "Attenuation" };
    }
private:
  // Feedback
  virtual int cc51( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Brightness
  virtual int cc52( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Attenuation
  virtual int cc53( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
  // no-op
  virtual int cc54( int value, unsigned char *cmd ) { return -1;}
};

class MonoEchoDelayCC: public MonoDelayCC {
    public:
  MonoEchoDelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : MonoDelayCC(theAmp,model,theSlot) {
      name = "Mono Echo Delay";
      paramName = { "Level", "Delay Time", "Feedback", "Frequency", "Resonance", "Input Level" };
    }
};

class StereoEchoFilterCC: public MonoDelayCC {
    public:
  StereoEchoFilterCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : MonoDelayCC(theAmp,model,theSlot) {
      name = "Stereo Echo Filter";
      paramName = { "Level", "Delay Time", "Feedback", "Frequency", "Resonance", "Input Level" };
    }
};

class EchoFilterCC : public DelayCC {
public:
  EchoFilterCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : DelayCC(theAmp,model,theSlot) {}
private:
  // Feedback
  virtual int cc51( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Frequency
  virtual int cc52( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Resonance
  virtual int cc53( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
  // Input Level
  virtual int cc54( int value, unsigned char *cmd ) { return continuous_control( 0x05, 0x05, 0x01, value, cmd );}
};


class MultitapDelayCC : public DelayCC {
public:
  MultitapDelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : DelayCC(theAmp,model,theSlot) {
      name = "Multitap Delay";
      paramName = { "Level", "Delay Time", "Feedback", "Brightness", "Mode" };
  }
private:
  // Delay Time
  virtual int cc50( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x08, value, cmd );}
  // Feedback
  virtual int cc51( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Brightness
  virtual int cc52( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Mode
  virtual int cc53( int value, unsigned char *cmd ) { 
    if ( value > 3 ) return -1;
    else             return discrete_control( 0x04, 0x04, 0x8b, value, cmd );
  }
  // no-op
  virtual int cc54( int value, unsigned char *cmd ) { return -1;}
};


class PingPongDelayCC : public DelayCC {
public:
  PingPongDelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : DelayCC(theAmp,model,theSlot) {
      name = "Ping Pong Delay";
      paramName = { "Level", "Delay Time", "Feedback", "Brightness", "Stereo" };
  }
private:
  // Feedback
  virtual int cc51( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Brightness
  virtual int cc52( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Stereo
  virtual int cc53( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
  // no-op
  virtual int cc54( int value, unsigned char *cmd ) { return -1;}
};


class DuckingDelayCC : public DelayCC {
public:
  DuckingDelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : DelayCC(theAmp,model,theSlot) {
      name = "Ducking Delay";
      paramName = { "Level", "Delay Time", "Feedback", "Release", "Threshold" };
  }
private:
  // Feedback
  virtual int cc51( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Release
  virtual int cc52( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Threshold
  virtual int cc53( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
  // no-op
  virtual int cc54( int value, unsigned char *cmd ) { return -1;}
};


class ReverseDelayCC : public DelayCC {
public:
  ReverseDelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : DelayCC(theAmp,model,theSlot) {
      name = "Reverse Delay";
      paramName = { "Level", "Delay Time", "FFdbk", "RFdbk", "Tone" };
  }
private:
  // FFdbk
  virtual int cc51( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // RFdbk
  virtual int cc52( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Tone
  virtual int cc53( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
  // no-op
  virtual int cc54( int value, unsigned char *cmd ) { return -1;}
};


class TapeDelayCC : public DelayCC {
public:
  TapeDelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : DelayCC(theAmp,model,theSlot) {
      name = "Tape Delay";
      paramName = { "Level", "Delay Time", "Feedback", "Flutter", "Brightness", "Stereo" };
  }
private:
  // Feedback
  virtual int cc51( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Flutter
  virtual int cc52( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Brightness
  virtual int cc53( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
  // Stereo
  virtual int cc54( int value, unsigned char *cmd ) { return continuous_control( 0x05, 0x05, 0x01, value, cmd );}
};


class StereoTapeDelayCC : public DelayCC {
public:
  StereoTapeDelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : DelayCC(theAmp,model,theSlot) {
      name = "Stereo Tape Delay";
      paramName = { "Level", "Delay Time", "Feedback", "Flutter", "Separation", "Brightness" };
  }
private:
  // Feedback
  virtual int cc51( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Flutter
  virtual int cc52( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Separation
  virtual int cc53( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x05, 0x01, value, cmd );}
  // Brightness
  virtual int cc54( int value, unsigned char *cmd ) { return continuous_control( 0x05, 0x04, 0x01, value, cmd );}
};


class NullDelayCC : public DelayCC {
public:
  NullDelayCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : DelayCC(theAmp,model,theSlot) {
      name = "None";
      paramName = {};
  }
private:
  virtual int cc49( int value, unsigned char *cmd ) { return -1;}
  virtual int cc50( int value, unsigned char *cmd ) { return -1;}
  virtual int cc51( int value, unsigned char *cmd ) { return -1;}
  virtual int cc52( int value, unsigned char *cmd ) { return -1;}
  virtual int cc53( int value, unsigned char *cmd ) { return -1;}
  virtual int cc54( int value, unsigned char *cmd ) { return -1;}
};


#endif
