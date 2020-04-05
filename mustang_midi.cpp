#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <RtMidi.h>
#include <cerrno>

#include "mustang.h"
#include "amp.h"
#include "stomp.h"
#include "delay.h"
#include "reverb.h"
#include "mod.h"

// If you see a compiler error complaining about a missing 
// symbol 'RtMidiError' you probably have an old version of
// of the library and will need to build with this flag:
// $ make CPPFLAGS=-DRTMIDI_2_0
//

#ifdef RTMIDI_2_0
# define RT_ERROR RtError
#else 
# define RT_ERROR RtMidiError
#endif

RtMidiOut *midi_out = NULL;

static Mustang mustang;

static int channel;

void handle_sysex_get_patches(std::vector< unsigned char > *message, void *userData ) {
    bool refresh = ((int)(*message)[2] > 63);

    if (refresh) {
        int rc = mustang.requestDump();
        if ( rc ) {
            fprintf( stderr, "requestDump failed  RC = %d\n", rc );
        }
    }
    
    Mustang::PresetNames *preset_names = mustang.getPresetNames();
    int i, j;
    const int name_len = 32; // sizeof(preset_names->names[0]);
    unsigned char msg[2+name_len*300+1];
    unsigned char *pos = msg;

    *pos++ = 0xF0;
    *pos++ = 0x01;
    for (i=0; i<(sizeof(preset_names->names)/name_len); i++) {
        if (preset_names->names[i][0] != '\0')
        {
            strncpy((char *)pos, preset_names->names[i], name_len);
	    fprintf(stderr, "[%s]\n", pos);
            pos += strlen((char *)pos);
            pos++;
        }
    }
    *pos++ = 0xF7;
    midi_out->sendMessage(msg, pos-msg);
}

void handle_sysex(std::vector< unsigned char > *message, void *userData ) {
    int msgType = (int)(*message)[1];
    switch (msgType) {
    case 0x01: // Get patchnames
        handle_sysex_get_patches(message, userData );
        break;
    }
}

void send_sysex(const char *msg, size_t len) {
    char dmsg[1+len+1];
    char *pos = dmsg;

    *pos++ = 0xF0;
    memcpy(pos, msg, len);
    pos += len;
    *pos++ = 0xF7;
    midi_out->sendMessage((const unsigned char *)dmsg, pos-dmsg);
}

void message_out_action(AmpEvent *ev) {
#ifdef DEBUG
    fprintf(stderr, "event: ev=%p type=%d pint1=%d\n", ev, ev->type, ev->pint1);
#endif
    FX *fx = (FX *)ev->ptr;
    string json;
    switch (ev->type) {
    case AmpEvent::PatchChanged:
        json = ev->str_val;
        /*fprintf(stderr, "PatchChanged patch=%d\n", ev->pint1);
        int idx = ev->pint1;
        unsigned char msb = (ev->pint1 & 0x3f80) >> 7;
        unsigned char lsb = (ev->pint1 & 0x007f);
        unsigned char msg[] = { 0x02, msb, lsb };
        send_sysex(msg, sizeof(msg));*/
        break;
    case AmpEvent::AmpChanged:
    case AmpEvent::StompChanged:
    case AmpEvent::DelayChanged:
    case AmpEvent::ModChanged:
    case AmpEvent::ReverbChanged:
        json = fx->to_json();
        break;
    default:
        fprintf(stderr, "Unhandled event type=%d\n", ev->type);
        return;
        break;
    }
    // fprintf(stderr, "Changed -%s-\n", json.c_str());
    send_sysex(json.c_str(), json.length());
}

void message_action( double deltatime, std::vector< unsigned char > *message, void *userData ) {
#if 0
  unsigned int nBytes = message->size();
  if      ( nBytes == 2 ) fprintf( stdout, "%02x %d\n", (int)message->at(0), (int)message->at(1) );
  else if ( nBytes == 3 ) fprintf( stdout, "%02x %d %d\n", (int)message->at(0), (int)message->at(1), (int)message->at(2) );
#endif

  // Is this for us?
  int msg_channel = (*message)[0] & 0x0f;

  int msg_type = (*message)[0] & 0xf0;

#ifdef DEBUG
  fprintf(stderr, "message: dtime=%.4f channel=%d type=%02x data=[%02x %02x]\n",
		  deltatime, msg_channel, msg_type, (*message)[1], (*message)[2]);
#endif

  if ( msg_channel != channel ) return;

  switch ( msg_type ) {

  case 0xc0: {
    // Program change
    int bank = (int)(*message)[1];

    int retries = 16;
    while (retries--) {
    	int rc = mustang.patchChange( bank );
    	if ( rc ) {
      		fprintf( stderr, "Error: PC#%d failed. RC = %d\n", bank, rc );
    	} else {
		break;
	}
	// usleep(10);
    }

#if 0
    int rc = mustang.patchChange( bank );
    if ( rc ) {
      fprintf( stderr, "Error: PC#%d failed. RC = %d\n", bank, rc );
    }
#endif
  }
  break;
    
  case 0xb0: {
    // Control change
    int rc = 0;
    int cc = (*message)[1];
    int value = (*message)[2];
    
    // Tuner toggle
    if ( cc == 20 ) {
      rc = mustang.tunerMode( value );
    }
    // All EFX toggle
    else if ( cc == 22 ) {
      rc = mustang.effectToggle( 23, value );
      if ( rc == 0 ) {
        rc = mustang.effectToggle( 24, value );
        if ( rc == 0 ) {
          rc = mustang.effectToggle( 25, value );
          if ( rc == 0 ) {
            rc = mustang.effectToggle( 26, value );
          }
        }
      }
    }
    // Effects on/off
    else if ( cc >= 23 && cc <= 26 ) {
      rc = mustang.effectToggle( cc, value );
    }
    // Set stomp model
    else if ( cc == 28 ) {
      rc = mustang.setStomp( value );
    }
    // Stomp CC handler
    else if ( cc >= 29 && cc <= 33 ) {
      rc = mustang.stompControl( cc, value );
    }
    // Set mod model
    else if ( cc == 38 ) {
      rc = mustang.setMod( value );
    }
    // Mod CC handler
    else if ( cc >= 39 && cc <= 43 ) {
      rc = mustang.modControl( cc, value );
    }
    // Set delay model
    else if ( cc == 48 ) {
      rc = mustang.setDelay( value );
    }
    // Delay CC handler
    else if ( cc >= 49 && cc <= 54 ) {
      rc = mustang.delayControl( cc, value );
    }
    // Set reverb model
    else if ( cc == 58 ) {
      rc = mustang.setReverb( value );
    }
    // Reverb CC handler
    else if ( cc >= 59 && cc <= 63 ) {
      rc = mustang.reverbControl( cc, value );
    }
    // Set amp model
    else if ( cc == 68 ) {
      rc = mustang.setAmp( value );
    }
    // Amp CC Handler
    else if ( cc >= 69 && cc <= 79 ) {
      rc = mustang.ampControl( cc, value );
    }
    if ( rc ) {
      fprintf( stderr, "Error: CC#%d failed. RC = %d\n", cc, rc );
    }
  }
  break;
  case 0xf0: // sysex message
    handle_sysex(message, userData);
  
  break;
 
  default:
    break;
  }

}


void usage() {
  const char msg[] = 
    "Usage: mustang_midi <controller_port#> <midi_channel#>\n"
    "       mustang_midi <virtual_port> <midi_channel#>\n\n"

    "       port = 0..n,  channel = 1..16\n";

  fprintf( stderr, msg );
  exit( 1 );
}


int main( int argc, const char **argv ) {
  if ( argc != 4 ) usage();

  RtMidiIn input_handler(RtMidi::UNSPECIFIED, "mustang-midi-bridge");
  midi_out = new RtMidiOut(RtMidi::UNSPECIFIED, "mustang-midi-bridge2");

  char *endptr;

  int port = (int) strtol( argv[1], &endptr, 10 );
  if ( endptr == argv[0] ) {
    try {
      input_handler.openVirtualPort( argv[2] );
    }
    catch ( RT_ERROR &error ) {
      exit( 1 );
    }
  }
  else {
    if ( port < 0 ) usage();
    try {
      input_handler.openPort( port, "control-input" );
    }
    catch ( RT_ERROR &error ) {
      exit( 1 );
    }
  }

  port = (int) strtol( argv[3], &endptr, 10 );
  if ( endptr == argv[3] ) {
    try {
      midi_out->openVirtualPort( argv[3] );
    }
    catch ( RT_ERROR &error ) {
      exit( 1 );
    }
  }
  else {
    if ( port < 0 ) usage();
    try {
      midi_out->openPort( 0, "control-output" ); // was port
    }
    catch ( RT_ERROR &error ) {
      exit( 1 );
    }
  }

  channel = (int) strtol( argv[2], &endptr, 10 ) - 1;
  if ( endptr == argv[0] ) usage();
  if ( channel < 0 || channel > 15 ) usage();
  
  input_handler.setCallback( &message_action );
  mustang.setEventCallback( &message_out_action );

  // Don't want sysex, timing, active sense
  input_handler.ignoreTypes( false, true, true );

  fprintf( stderr, "Mustang initializing...\n" );
  if ( 0 != mustang.initialize() ) {
    fprintf( stderr, "Cannot setup USB communication\n" );
    exit( 1 );
  }
  fprintf( stderr, "Starting comms...\n" );
  if ( 0 != mustang.commStart() ) {
    fprintf( stderr, "Thread setup and init failed\n" );
    exit( 1 );
  }

  fprintf( stderr, "Requesting dump...\n" );
  if (0 != mustang.requestDump()) {
    fprintf( stderr, "Request dump failed\n" );
    exit( 1 );
  }
  // Block and wait for signal 
  pause();
  fprintf(stderr, "Caught signal... shutting down\n");

  if ( 0 != mustang.commShutdown() ) {
    fprintf( stderr, "Thread shutdown failed\n" );
    exit( 1 );
  }
  if ( 0 != mustang.deinitialize() ) {
    fprintf( stderr, "USB shutdown failed\n" );
    exit( 1 );
  }
  
  // delete input_handler;
  return 0;

}
