#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>

#ifdef WITHSDL
#include <SDL2/SDL.h>
#include <stdint.h>
#include <thread>
#endif


#include <signal.h>
#include "structs.h"
#include "debug.h"
#include "sdlscreen.h"
#include "simulateur.h"
#include "utilities.h"



bool force_quit = false;
/* function to handle any forced exit */
void handleForceExit(int sig) {
	force_quit = true;
	exit(sig);
}

void tolowercase(std::string &data) {
  std::transform(data.begin(), data.end(), data.begin(), ::tolower);
}

void loadClockTicksRc(const std::string &dir, struct ClockTicks &ct) {
  std::ifstream file(dir + std::string("/config.rc"));
  if (!file.is_open() || file.fail()) {
    std::cout << "Exception reading config.rc\n";
    return; 
  } else {
    std::string keyword;
    unsigned int value;
    while (file >> keyword >> value) {
      tolowercase(keyword);
      if (keyword == "add")
	ct.add_t = value;
      else if (keyword == "sub")
	ct.sub_t = value;
      else if (keyword == "wmem")
	ct.wmem_t = value;
      else if (keyword == "snif")
	ct.snif_t = value;
      else if (keyword == "and")
	ct.and_t = value;
      else if (keyword == "or")
	ct.or_t = value;
      else if (keyword == "xor")
	ct.xor_t = value;
      else if (keyword == "lsl")
	ct.lsl_t = value;
      else if (keyword == "lsr")
	ct.lsr_t = value;
      else if (keyword == "asr")
	ct.asr_t = value;
      else if (keyword == "call")
	ct.call_t = value;
      else if (keyword == "jump")
	ct.jump_t = value;
      else if (keyword == "return")
	ct.return_t = value;
      else if (keyword == "letl")
	ct.letl_t = value;
      else if (keyword == "leth")
	ct.leth_t = value;
      else if (keyword == "copy")
	ct.copy_t = value;
      else if (keyword == "rmem")
	ct.rmem_t = value;
      else if (keyword == "ticks")
	ct.click_constant = value;

    }
    // std::cout<<keyword<<": "<<value<<"\n";
    
  }
  // std::cout<<"file opened";
  file.close();
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Syntax: %s {-s,-r,-f, -fulldebug,-q} <file.obj>\n", argv[0]);
    printf("-s: step by step\n"
	   "-r: recursive\n"
	   "-f: fast simulation\n"
	   "-fulldebug: verbose++ mode\n"
	   "-q: quiet mode, no graphical screen\n");
    return EXIT_FAILURE;
  }
  ClockTicks ct = clockticks_new();
  {
    using namespace std;
    string dir = argv[0];
    int pos = dir.rfind('/');
    if (pos != -1) {
      dir.resize(pos); // dirname of argv[0],
      // i.e. LEIA directory.
      loadClockTicksRc(dir, ct);
    } else {
      // We couldn't find the .rc file. Use hardcoded default.
      ct.click_constant = 0;
      ct.add_t = 1;
    }
  }
  Param param;
  bool quiet = false;
  /* parse the params */
  if (std::string(argv[1]) == "-s") {
    param.step_by_step = true;
    param.debug_output = true;
    param.fast_mode = false;
    param.full_debug = false;
    param.skip_call = false;
  } else if (std::string(argv[1]) == "-skip") {
    param.step_by_step = true;
    param.debug_output = true;
    param.fast_mode = false;
    param.full_debug = false;
    param.skip_call = true;
  } else if (std::string(argv[1]) == "-q") {
    param.step_by_step = false;
    param.debug_output = false;
    param.fast_mode = true;
    param.full_debug = false;
    param.skip_call = false;
    quiet = true;
  } else if (std::string(argv[1]) == "-r") {
    param.step_by_step = false;
    param.debug_output = true;
    param.fast_mode = false;
    param.full_debug = false;
    param.skip_call = false;
  } else if (std::string(argv[1]) == "-fulldebug") {
    param.step_by_step = false;
    param.debug_output = false;
    param.fast_mode = false;
    param.full_debug = true;
    param.skip_call = false;
  }
  else {
    param.step_by_step = false;
    param.debug_output = false;
    param.fast_mode = true;
    param.full_debug = false;
    param.skip_call = false;
  }

  /* add interruptions for Ctrl+Z or Ctrl+C */
  /* bad way to deal with signals, but I learned the good way later in time. Shame on me*/
  signal(SIGINT, handleForceExit);
  signal(SIGTSTP, handleForceExit);

  Machine machine;
  machine.clock_ticks = ct;
  machine.in_call = false;
  
  /* if SDL: launch the screen with the option to manually refresh the deactivated screen*/
  Screen *screen_ = 0;
#ifdef WITHSDL
  if (!quiet) {
    printf("Graphical screen: on\n");
    screen_ = new SDLScreen;
  }
#else
    if (!quiet) {
    printf("Graphical screen: off (SDL compilation has been disabled)\n");
    }
#endif
	/* if we can read the program */
	if (readFromStr(argv[2], machine)) {
		loadCodeToMemory(machine);
		clock_t t_begin = clock();
		if (!param.full_debug) {
		  simulate(machine, param, screen_);
		} else {
		  fullDebug(machine, param, screen_);
		}
		if (!quiet) {
		  clock_t t_end = clock();
		  double time_spent = (double)(t_end - t_begin) / CLOCKS_PER_SEC;
		  printf("Simulation finished in %f ms\n", time_spent);
		  printf("Please press the ENTER key");
		  getchar();
		  force_quit = true;
		  printf("\n");
		}
	}
	
	if (!quiet) {
	  delete screen_;
	  printf("\n");
	}
	return EXIT_SUCCESS;
}
