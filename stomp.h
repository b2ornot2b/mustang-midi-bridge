// -*-c++-*-

#ifndef _STOMP_H
#define _STOMP_H

#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "fx.h"

using namespace std;

class Mustang;

class StompCC : public FX {

protected:
  Mustang * amp;
  unsigned char model[2];
  unsigned char slot;
  
  int continuous_control( int parm5, int parm6, int parm7, int value, unsigned char *cmd );
  int discrete_control( int parm5, int parm6, int parm7, int value, unsigned char *cmd );


public:
  string name;
  vector<string> paramName;
  vector<int> param;

  StompCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : 
    amp(theAmp), 
    slot(theSlot) 
  {
    memcpy( this->model, model, 2 );
  }

  int dispatch( int cc, int value, unsigned char *cmd );
  const unsigned char *getModel( void ) { return model;}
  const unsigned char getSlot( void ) { return slot;}



  const std::string to_json(void) {
    std::stringstream ss;
    ss   << "{ \"name\": \"" << name << "\", "
         << " \"type\": \"Stomp\", "
         << "  \"params\": { ";
    for (auto i=0; i < paramName.size(); i++) {
        if (i) ss << ",";
        ss << " \"" << paramName[i] << "\": " << param[i] << " ";
    }
    ss   << "}}";
    return ss.str();
  }

private:
  virtual int cc29( int value, unsigned char *cmd ) = 0;
  virtual int cc30( int value, unsigned char *cmd ) = 0;
  virtual int cc31( int value, unsigned char *cmd ) = 0;
  virtual int cc32( int value, unsigned char *cmd ) = 0;
  virtual int cc33( int value, unsigned char *cmd ) = 0;
 };



class OverdriveCC : public StompCC {
public:
  OverdriveCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
    name = "Overdrive";
    paramName = { "Level", "Gain", "Low", "Mid", "High" };
}
private:
  // Level
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Gain
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x01, value, cmd );}
  // Low
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Mid
  virtual int cc32( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // High
  virtual int cc33( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
};

class WahCC : public StompCC {
public:
  WahCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
    name = "Wah";
    paramName = { "Mix", "Frequency", "Heel Freq", "Toe Freq", "High Q" };
 }
private:
  // Mix
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Freq
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x01, value, cmd );}
  // Heel Freq
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Toe Freq
  virtual int cc32( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // High Q
  virtual int cc33( int value, unsigned char *cmd ) { 
    if ( value > 1 ) return -1;
    else             return discrete_control( 0x04, 0x04, 0x81, value, cmd );
  }
};


class FuzzCC : public StompCC {
public:
  FuzzCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
    name = "Fuzz";
    paramName = { "Level", "Gain", "Sensitivity", "Octave", "Peak" };
  }
private:
  // Level
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Gain
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x01, value, cmd );}
  // Octave
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Low
  virtual int cc32( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // High
  virtual int cc33( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
};


class FuzzTouchWahCC : public StompCC {
public:
  FuzzTouchWahCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
     name = "Fuzz Touch Wha";
     paramName = { "Level", "Gain", "Sensitivity", "Octave", "Peak" };
  }
private:
  // Level
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Gain
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x01, value, cmd );}
  // Sens
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Octave
  virtual int cc32( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Peak
  virtual int cc33( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
};


class SimpleCompCC : public StompCC {
public:
  SimpleCompCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
      name = "Compressor";
      paramName = { "Type" };
  }
private:
  // Type
  virtual int cc29( int value, unsigned char *cmd ) { 
    if ( value > 3 ) return -1;
    else             return discrete_control( 0x00, 0x05, 0x92, value, cmd );
  }
  virtual int cc30( int value, unsigned char *cmd ) { return -1;}
  virtual int cc31( int value, unsigned char *cmd ) { return -1;}
  virtual int cc32( int value, unsigned char *cmd ) { return -1;}
  virtual int cc33( int value, unsigned char *cmd ) { return -1;}
};


class CompCC : public StompCC {
public:
  CompCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
    name = "Compressor";
    paramName = { "Level", "Threshold", "Ratio", "Attack Time", "Release Time" };
  }
private:
  // Level
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Thresh
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x01, value, cmd );}
  // Ratio
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x04, value, cmd );}
  // Attack
  virtual int cc32( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x01, value, cmd );}
  // Release
  virtual int cc33( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x01, value, cmd );}
};


class RangerCC : public StompCC {
public:
  RangerCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot) : StompCC(theAmp,model,theSlot) {
      name = "Ranger Boost";
      paramName = { "Level", "Gain", "Lo-cut", "Bright" };
  }
private:
  // Level
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Gain
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x01, value, cmd );}
  // Lo-Cut
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x03, 0x01, value, cmd );}
  // Bright
  virtual int cc32( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x02, 0x01, value, cmd );}
  // n/a
  virtual int cc33( int value, unsigned char *cmd ) { return -1;}
};


class GreenBoxCC : public StompCC {
public:
  GreenBoxCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
      name = "Green Box";
      paramName = { "Level", "Gain", "Tone", "Blend" };
  }
private:
  // Level
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Gain
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x01, value, cmd );}
  // Tone
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // Bright
  virtual int cc32( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x12, value, cmd );}
  // n/a
  virtual int cc33( int value, unsigned char *cmd ) { return -1;}
};


class OrangeBoxCC : public StompCC {
public:
  OrangeBoxCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
      name = "Orange Box";
      paramName = { "Level", "Dist", "Tone" };
    }
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Dist
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x02, 0x01, value, cmd );}
  // Tone
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x01, 0x01, value, cmd );}
  // n/a
  virtual int cc32( int value, unsigned char *cmd ) { return -1;}
  // n/a
  virtual int cc33( int value, unsigned char *cmd ) { return -1;}
};


class BlackBoxCC : public StompCC {
public:
  BlackBoxCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
      name = "Black Box";
        paramName = { "Level", "Dist", "Filter" };
  }
private:
  // Level
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Dist
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x02, 0x01, value, cmd );}
  // Filter
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x01, 0x01, value, cmd );}
  // n/a
  virtual int cc32( int value, unsigned char *cmd ) { return -1;}
  // n/a
  virtual int cc33( int value, unsigned char *cmd ) { return -1;}
};


class BigFuzzCC : public StompCC {
public:
  BigFuzzCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
      name = "Big Fuzz";
      paramName = { "Level", "Tone", "Sustain" };
  }
private:
  // Level
  virtual int cc29( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x01, value, cmd );}
  // Tone
  virtual int cc30( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x01, value, cmd );}
  // Sustain
  virtual int cc31( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x01, value, cmd );}
  // n/a
  virtual int cc32( int value, unsigned char *cmd ) { return -1;}
  // n/a
  virtual int cc33( int value, unsigned char *cmd ) { return -1;}
};


class NullStompCC : public StompCC {
public:
  NullStompCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : StompCC(theAmp,model,theSlot) {
      name = "None";
      paramName = {};
  }
private:
  virtual int cc29( int value, unsigned char *cmd ) { return -1;}
  virtual int cc30( int value, unsigned char *cmd ) { return -1;}
  virtual int cc31( int value, unsigned char *cmd ) { return -1;}
  virtual int cc32( int value, unsigned char *cmd ) { return -1;}
  virtual int cc33( int value, unsigned char *cmd ) { return -1;}
};


#endif
