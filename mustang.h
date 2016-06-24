// -*-c++-*-

#ifndef MUSTANG_H
#define MUSTANG_H

#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include "effects_enum.h"
#include "data_structs.h"
#include "amp.h"
#include "reverb.h"

// amp's VID and PID
#define USB_VID 0x1ed8
#define OLD_USB_PID 0x0004  //Mustang I and II
#define NEW_USB_PID 0x0005  //Mustang III, IV and V
#define V2_USB_PID 0x0014   // Mustang II (and I? !) V2
#define MINI_USB_PID 0x0010 //Mustang Mini
#define FLOOR_USB_PID 0x0012 //Mustang Floor
#define BRONCO40_USB_PID 0x0a //BRONCO 40
#define V2_III_PID 0x16 // Mustang III v2
#define V2_IV_PID 0x17 // Mustang IV v2 (?)

// amp's VID and PID while in update mode
#define USB_UPDATE_VID 0x1ed8
#define OLD_USB_UPDATE_PID 0x0006  //Mustang I and II
#define NEW_USB_UPDATE_PID 0x0007  //Mustang III, IV, V
#define MINI_USB_UPDATE_PID 0x0011 //Mustang Mini
#define FLOOR_USB_UPDATE_PID 0x0013 //Mustang Floor
// #define BRONCO40_USB_PID 0x00a //BRONCO 40


// for USB communication
#define TMOUT 500
#define LENGTH 64
//#define NANO_SEC_SLEEP 10000000

// effect array fields
#define DSP 2
#define EFFECT 16
#define FXSLOT 18
#define KNOB1 32
#define KNOB2 33
#define KNOB3 34
#define KNOB4 35
#define KNOB5 36
#define KNOB6 37

// amp array fields
#define AMPLIFIER 16
#define VOLUME 32
#define GAIN 33
#define TREBLE 36
#define MIDDLE 37
#define BASS 38
#define CABINET 49
#define NOISE_GATE 47
#define THRESHOLD 48
#define MASTER_VOL 35
#define GAIN2 34
#define PRESENCE 39
#define DEPTH 41
#define BIAS 42
#define SAG 51
#define BRIGHTNESS 52

// save fields
#define SAVE_SLOT 4
#define FXKNOB 3

// direct control fields
#define FAMILY 2
#define ACTIVE_INVERT 3

// Index into current state structure
#define PRESET_NAME  0
#define AMP_STATE    1
#define STOMP_STATE  2
#define MOD_STATE    3
#define DELAY_STATE  4
#define REVERB_STATE 5
#define EXP_STATE    6

// DSP Category
#define AMP_DSP      5
#define STOMP_DSP    6
#define MOD_DSP      7
#define DELAY_DSP    8
#define REVERB_DSP   9

// DSP Family (used for direct parm set) - Effectively DSP - 3
#define STOMP_FAM    3
#define MOD_FAM      4
#define DELAY_FAM    5
#define REVERB_FAM   6

// Amp id values
#define F57_DELUXE_ID    0x67
#define F59_BASSMAN_ID   0x64
#define F57_CHAMP_ID     0x7c
#define F65_DELUXE_ID    0x53
#define F65_PRINCETON_ID 0x6a
#define F65_TWIN_ID      0x75
#define F_SUPERSONIC_ID  0x72
#define BRIT_60S_ID      0x61
#define BRIT_70S_ID      0x79
#define BRIT_80S_ID      0x5e
#define US_90S_ID        0x5d
#define METAL_2K_ID      0x6d
#define STUDIO_PREAMP_ID 0xf1

#define F57_TWIN_ID      0xf6
#define S60S_THRIFT_ID   0xf9
#define BRIT_WATT_ID     0xff
#define BRIT_COLOR_ID    0xfc

// Reverb id values
#define SM_HALL_ID       0x24
#define LG_HALL_ID       0x3a
#define SM_ROOM_ID       0x26
#define LG_ROOM_ID       0x3b
#define SM_PLATE_ID      0x4e
#define LG_PLATE_ID      0x4b
#define AMBIENT_ID       0x4c
#define ARENA_ID         0x4d
#define SPRING_63_ID     0x21
#define SPRING_65_ID     0x0b

class Mustang
{
public:
    Mustang();
    ~Mustang();
    int start_amp(void);    // initialize communication
    int stop_amp(void);    // terminate communication
    int set_effect(struct fx_pedal_settings);
    int setAmp( int ord );

    int setReverb( int ord );

    int save_on_amp(char *, int);
    int load_memory_bank(int);
    int save_effects(int , char *, int , struct fx_pedal_settings *);

    // State:  1 = off, 0 = on
    int effect_toggle(int category, int state);

    int control_common1(int parm, int bucket, int value);
    int control_common2(int parm, int bucket, int value);

    int efx_common1(int parm, int bucket, int type, int value);
    
    AmpCC * getAmp( void ) { return curr_amp;}
    ReverbCC * getReverb( void ) { return curr_reverb;}

 private:
    libusb_device_handle *amp_hand;    // handle for USB communication
    unsigned char execute[LENGTH];    // "apply" command sent after each instruction

    // Current state of effects. Read from amp initially and maintained
    // as we change it. Major index is DSP# - 6, where:
    //
    // 0 : Stomp
    // 1 : Modulation
    // 2 : Delay
    // 3 : Reverb
    //
    unsigned char prev_array[4][LENGTH];    // array used to clear the effect

    
    // Current state of amp.
    //
    // 0 : Preset Name
    // 1 : Amp
    // 2 : Stomp
    // 3 : Mod
    // 4 : Delay
    // 5 : Reverb
    // 6 : Expression Pedal
    //
    unsigned char curr_state[7][LENGTH];
    
    AmpCC * curr_amp;
    ReverbCC * curr_reverb;
    
    void updateAmp(void);
    void updateReverb(void);

};

#endif // MUSTANG_H
