// -*-c++-*-

#ifndef _FX_H
#define _FX_H

#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

class Mustang;

class FX {

protected:
  Mustang * amp;

public:
#if 0
  string name;
  vector<string> paramName;

  FX( Mustang * theAmp ) : 
    amp(theAmp), 
  {
  }
#endif
  virtual const std::string to_json(void) = 0;

};

#endif
