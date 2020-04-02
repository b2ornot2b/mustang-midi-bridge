// -*-c++-*-

#ifndef _REVERB_H
#define _REVERB_H

#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

class Mustang;

class ReverbCC {

protected:
  Mustang * amp;
  unsigned char model[2];
  unsigned char slot;

  int continuous_control( int parm5, int parm6, int parm7, int value, unsigned char *cmd );

  string name;
  vector<string> paramName;

public:
  ReverbCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : 
    amp(theAmp), 
    slot(theSlot) 
  {
    memcpy( this->model, model, 2 );
    cout << "\n\nREVERB --- " << std::hex << int(model[0]) << " " << std::hex << int(model[1]) << endl;
    switch (model[0]) {
    case 0x4c:
        name = "Ambient";
        break;
    case 0x4d:
        name = "Arena";
        break;
    case 0x21:
        name = "63 Fender Spring";
        break;
    case 0x0b:
        name = "65 Fender Sprint";
        break;
    case 0x4b:
        name = "Large Plate";
        break;
    case 0x4e:
        name = "Small Plate";
        break;
    case 0x3b:
        name = "Large Room";
        break;
    case 0x26:
        name = "Small Room";
        break;
    case 0x3a:
        name = "Large Hall";
        break;
    case 0x24:
        name = "Small Hall";
        break;
    default:
        name = "Unknown";
        break;
    } 
    
    paramName = { "Level", "Decay", "Dwell", "Diffusion", "Tone" };
  }

  int dispatch( int cc, int value, unsigned char *cmd );
  const unsigned char *getModel( void ) { return model;}
  const unsigned char getSlot( void ) { return slot;}

  const std::string to_json(void) {
    std::stringstream ss;
    ss   << "{ \"name\": \"" << name << "\", "
         << " \"type\": \"Reverb\", "
         << "  \"params\": { ";
    for (auto i=0; i < paramName.size(); i++) {
        if (i) ss << ",";
        ss << " \"" << paramName[i] << "\": " << param[i] << " ";
    }
    ss   << "}}";
    return ss.str();
  }

  vector<int> param;


private:
  // Level
  int cc59( int value, unsigned char *cmd ) { return continuous_control( 0x00, 0x00, 0x0b, value, cmd );}
  // Decay
  int cc60( int value, unsigned char *cmd ) { return continuous_control( 0x01, 0x01, 0x0b, value, cmd );}
  // Dwell
  int cc61( int value, unsigned char *cmd ) { return continuous_control( 0x02, 0x02, 0x0b, value, cmd );}
  // Diffusion
  int cc62( int value, unsigned char *cmd ) { return continuous_control( 0x03, 0x03, 0x0b, value, cmd );}
  // Tone
  int cc63( int value, unsigned char *cmd ) { return continuous_control( 0x04, 0x04, 0x0b, value, cmd );}
};


class NullReverbCC : public ReverbCC {
public:
  NullReverbCC( Mustang * theAmp, const unsigned char *model, const unsigned char theSlot ) : ReverbCC(theAmp,model,theSlot) {
      name = "None";
      paramName = { };
  }
private:
  int cc59( int value, unsigned char *cmd ) { return -1;}
  int cc60( int value, unsigned char *cmd ) { return -1;}
  int cc61( int value, unsigned char *cmd ) { return -1;}
  int cc62( int value, unsigned char *cmd ) { return -1;}
  int cc63( int value, unsigned char *cmd ) { return -1;}
};


#endif
