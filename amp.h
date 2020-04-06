// -*-c++-*-

#ifndef _AMPCC_H
#define _AMPCC_H

#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "fx.h"

using namespace std;

class Mustang;

// F57 Deluxe
// F57 Champ
// F65 Deluxe
// F65 Princeton
// 60s Thrift
//
class AmpCC : public FX {

protected:
  Mustang * amp;
  unsigned char model[2];
  unsigned char slot;

  int continuous_control( int parm5, int parm6, int parm7, int value, unsigned char *cmd );
  int discrete_control( int parm5, int parm6, int parm7, int value, unsigned char *cmd );

public:
  string name;
  vector<int> param;
  vector<int> paramCC;
  vector<string> paramName;

  AmpCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : 
    amp(theAmp), 
    slot(theSlot) 
  {
    memcpy( this->model, model, 2 );
    printf("################ Model 0x%02x 0x%02x\n", model[0], model[1]);
    paramName = { 
        "Volume", // 0
        "Gain", 
        "Blend", 
        "Master_vol", 
        "Treble", 

        "Middle", // 5
        "Bass", 
        "Presence", 
        "", 
        "Depth", 

        "Bias", // 10
        "", "", "", "", 
        
        "Noise_gate", // 15 
        "Threshold", 
        "Cabinet", 
        "",
        "Sag", 

	"", "", "", "", ""// 20

    };
    paramCC = {  70, // vol
	    69, // gain
	    78, // gain2
	    79, // Master_vol
	    71, // Treble
	    72, // Middle
	    73, // Bass
	    78, // Presence
	    0, 
	    91, // Depth
	    75, // Bias
	    0, 0, 0, 0,
	    76, // Noise_gate
	    90, // Threshold
	    77, // Cabinet
	    0,
	    74, // Sag
    };
    // 70,  // Channel_vol
    // 92, // Brightness
    // 78, // Cut
    // 79, // Blend
    switch(model[0]) {
    	case 0xf1:
        	name = "Studio Preamp";
 		break;
	case 0xf6:
		name = "'57 Twin";
        break;
 	case 0x7c:
		name = "'57 Champ";
		break;
    case 0x67:
        name = "'57 Deluxe";
        break;
    case 0x64:
        name = "'59 Bassman";
        break;
    case 0x6a:
        name = "'65 Princeton";
        break;
    case 0x53:
        name = "'65 Deluxe Reverb";
        break;
    case 0x75:
        name = "'65 Twin Reverb";
        break;
    case 0xf9:
        name = "'60s Thrift";
        break;
    case 0xff:
        name = "British Watts";
        break;
    case 0x61:
        name = "British '60s";
        break;
    case 0x79:
        name = "British '70s";
        break;
    case 0x5e:
        name = "British '80s";
        break;
    case 0xfc:
        name = "British Colour";
        break;
    case 0x72:
        name = "Super-Sonic (Burn)";
        break;
    case 0x5d:
        name = "American '90s";
        break;
    case 0x6d:
	    name = "Metal 2000";
	    break;

    }
  } 
  int dispatch( int cc, int value, unsigned char *cmd );
  const unsigned char *getModel( void ) { return model;}
  const unsigned char getSlot( void ) { return slot;}

  const std::string to_json(void) {
    std::stringstream ss;
    ss   << "{ \"name\": \"" << name << "\", "
         << " \"type\": \"Amp\", "
	 << " \"cc\": 27, "
         << "  \"params\": { ";
    auto need_comma=false;
    for (auto i=0; i < paramName.size(); i++) {
        if (paramName[i].empty())
            continue;
        if (need_comma) ss << ",";
        need_comma = true;
        // ss << " \"" << paramName[i] << "\": " << param[i] << " ";
        ss << " \"" << paramName[i] << "\": ";
      	ss << "[" << param[i] << "," << std::dec << paramCC[i] << "]";
    }
    ss   << "}}";
    return ss.str();
  }

 

private:
  // Gain
  virtual int cc69( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x0c, value, cmd );}
  // Ch. Volume
  virtual int cc70( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x0c, value, cmd );}
  // Treble
  virtual int cc71( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x0c, value, cmd );}
  // Mid
  virtual int cc72( int value, unsigned char *cmd ) { return continuous_control( 0x05, 0x05, 0x0c, value, cmd );}
  // Bass
  virtual int cc73( int value, unsigned char *cmd ) { return continuous_control( 0x06, 0x06, 0x0c, value, cmd );}
  // Sag
  virtual int cc74( int value, unsigned char *cmd ) { 
    if ( value <= 2 ) return discrete_control( 0x13, 0x13, 0x8f, value, cmd );
    else              return -1;
  }
  // Bias
  virtual int cc75( int value, unsigned char *cmd ) { return continuous_control( 0x0a, 0x0a, 0x0d, value, cmd );}
  // Noise Gate
  virtual int cc76( int value, unsigned char *cmd ) { 
    if ( value <= 4 ) return discrete_control( 0x0f, 0x0f, 0x90, value, cmd );
    else              return -1;
  }
  // Cabinet
  virtual int cc77( int value, unsigned char *cmd ) {
    if ( value <= 12 ) return discrete_control( 0x11, 0x11, 0x8e, value, cmd );
    else               return -1;
  }

  // Dummy in base class
  virtual int cc78( int value, unsigned char *cmd ) { return -1;}
  virtual int cc79( int value, unsigned char *cmd ) { return -1;}

  // Noise Gate Custom Threshold
  virtual int cc90( int value, unsigned char *cmd ) { 
    if ( value <= 9 ) return discrete_control( 0x10, 0x10, 0x86, value, cmd );
    else              return -1;
  }
  // Noise Gate Custom Depth
  virtual int cc91( int value, unsigned char *cmd ) { return continuous_control( 0x09, 0x09, 0x0c, value, cmd );}
  // Dummy in base class
  virtual int cc92( int value, unsigned char *cmd ) { return -1;}
};


// F59 Bassman
// British 70s
//
class AmpCC1 : public AmpCC {
public:
  AmpCC1( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : AmpCC(theAmp,model,theSlot) {
    paramName[7] = "Presence";
    paramCC[7] = 78;

    paramName[2] = "Blend";
    paramCC[2] = 79;
  }
private:
  // Presence
  virtual int cc78( int value, unsigned char *cmd ) { return continuous_control( 0x07, 0x07, 0x0c, value, cmd );}
  // Blend
  virtual int cc79( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x0c, value, cmd );}
};
  

// Fender Supersonic
//
class AmpCC2 : public AmpCC {
public:
  AmpCC2( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : AmpCC(theAmp,model,theSlot) {
      paramName[2] = "Gain2";
      paramCC[2] = 78;

      paramName[3] = "Master_vol";
      paramCC[3] = 79;
  }
private:
  // Gain2
  virtual int cc78( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x0c, value, cmd );}
  // Master Volume
  virtual int cc79( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x0c, value, cmd );}
};
  

// British 60s
//
class AmpCC3 : public AmpCC {
public:
  AmpCC3( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : AmpCC(theAmp,model,theSlot) {
    paramName[0x07] = "Cut";
    paramCC[0x07] = 78;

    paramName[0x03] = "Master_vol";
    paramCC[0x03] = 78;

    paramName[0x14] = "Bright_sw";
    paramCC[0x14] = 92;
}
private:
  // Cut
  virtual int cc78( int value, unsigned char *cmd ) { return continuous_control( 0x07, 0x07, 0x0c, value, cmd );}
  // Master Volume
  virtual int cc79( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x0c, value, cmd );}
  // Bright Switch
  virtual int cc92( int value, unsigned char *cmd ) { 
    // Inverted logic
    unsigned char flag;
    // 0 --> Bright On
    if ( value>63 && value <=127 ) flag = 0;
    else                           flag = 1;
    return discrete_control( 0x14, 0x14, 0x8d, value, cmd );
  }
};
  

// British 80s
// American 90s
// Metal 2000
// British Watt
//
class AmpCC4 : public AmpCC {
public:
  AmpCC4( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : AmpCC(theAmp,model,theSlot) {
      paramName[0x07] = "Presence";
      paramCC[0x07] = 78;

      paramName[0x03] = "Master_vol";
      paramCC[0x03] = 79;
  }
private:
  // Presence
  virtual int cc78( int value, unsigned char *cmd ) { return continuous_control( 0x07, 0x07, 0x0c, value, cmd );}
  // Master Volume
  virtual int cc79( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x0c, value, cmd );}
};
  

// Studio Preamp
//
class AmpCC5 : public AmpCC {
public:
  AmpCC5( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : AmpCC(theAmp,model,theSlot) {
      paramName[19] = "";
      paramName[10] = "";
      paramName[7] = "";
      paramName[4] = "";
  }
private:
  // No sag / bias
  virtual int cc74( int value, unsigned char *cmd ) { return -1;}
  virtual int cc75( int value, unsigned char *cmd ) { return -1;}
  // No pres / master
  virtual int cc78( int value, unsigned char *cmd ) { return -1;}
  virtual int cc79( int value, unsigned char *cmd ) { return -1;}
};
  

// British Color
//
class AmpCC6 : public AmpCC {
public:
  AmpCC6( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : AmpCC(theAmp,model,theSlot) {
      paramName[0x03] = "Master_vol";
      paramCC[0x03] = 79;
  }
private:
  // Master Volume
  virtual int cc79( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x0c, value, cmd );}
};
  

// F57 Twin
//
class AmpCC7 : public AmpCC {
public:
  AmpCC7( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : AmpCC(theAmp,model,theSlot) {
      paramName[0x07] = "Presence";
      paramCC[0x07] = 78;
  }
private:
  // Presence
  virtual int cc78( int value, unsigned char *cmd ) { return continuous_control( 0x07, 0x07, 0x0c, value, cmd );}
};
  

// F65 Twin
//
class AmpCC8 : public AmpCC {
public:
  AmpCC8( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : AmpCC(theAmp,model,theSlot) {
      paramName[0x14] = "Bright_sw";
      paramCC[0x14] = 92;
  }
private:
  // Bright Switch
  virtual int cc92( int value, unsigned char *cmd ) { 
    // Inverted logic
    unsigned char flag;
    // 0 --> Bright On
    if ( value>63 && value <=127 ) flag = 0;
    else                           flag = 1;
    return discrete_control( 0x14, 0x14, 0x8d, value, cmd );
  }
};
  

// Null Amp
//
class NullAmpCC : public AmpCC {
public:
  NullAmpCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : AmpCC(theAmp,model,theSlot) {
      for (auto i=69; i<80; i++) {
          paramName[i] = "";
      }
  }
private:
  virtual int cc69( int value, unsigned char *cmd ) { return -1;}
  virtual int cc70( int value, unsigned char *cmd ) { return -1;}
  virtual int cc71( int value, unsigned char *cmd ) { return -1;}
  virtual int cc72( int value, unsigned char *cmd ) { return -1;}
  virtual int cc73( int value, unsigned char *cmd ) { return -1;}
  virtual int cc74( int value, unsigned char *cmd ) { return -1;}
  virtual int cc75( int value, unsigned char *cmd ) { return -1;}
  virtual int cc76( int value, unsigned char *cmd ) { return -1;}
  virtual int cc77( int value, unsigned char *cmd ) { return -1;}
  virtual int cc78( int value, unsigned char *cmd ) { return -1;}
  virtual int cc79( int value, unsigned char *cmd ) { return -1;}
};
  
#endif
