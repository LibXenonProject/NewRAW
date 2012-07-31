/* Raw - Another World Interpreter
 * Copyright (C) 2004 Gregory Montoir
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "engine.h"
#include "systemstub.h"
#include "util.h"
#include "xenon.h"
#include "mount.h"

static const char *USAGE = 
	"Raw - Another World Interpreter\n"
	"Usage: raw [OPTIONS]...\n"
	"  --datapath=PATH   Path to where the game is installed (default '.')\n"
	"  --savepath=PATH   Path to where the save files are stored (default '.')\n";

static bool parseOption(const char *arg, const char *longCmd, const char **opt) {
	bool ret = false;
	if (arg[0] == '-' && arg[1] == '-') {
		if (strncmp(arg + 2, longCmd, strlen(longCmd)) == 0) {
			*opt = arg + 2 + strlen(longCmd);
			ret = true;
		}
	}
	return ret;
}

#undef main
int main(int argc, char *argv[]) {
	
	char *basepath = (char*)malloc(256);
	
	if(argc != 0 && argv[0]){
		char *tmp = argv_GetFilepath(argv[0]);
		sprintf(basepath,"%s/raw",tmp);
	} else {
		basepath = "uda0:/raw";
	}
	
	xenon_make_it_faster(XENON_SPEED_FULL);
    xenos_init(VIDEO_MODE_AUTO);

	
    console_init();
    
    usb_init();
    usb_do_poll();
    
    xenon_ata_init();
    xenon_atapi_init();
    
    mount_all_devices();
    findDevices();
    
	xenon_sound_init();

	const char *dataPath = basepath;
	const char *savePath = basepath;
	for (int i = 1; i < argc; ++i) {
		bool opt = false;
		if (strlen(argv[i]) >= 2) {
			opt |= parseOption(argv[i], "datapath=", &dataPath);
			opt |= parseOption(argv[i], "savepath=", &savePath);
		}
		if (!opt) {
			printf(USAGE);
			return 0;
		}
	}
	g_debugMask = DBG_INFO; // DBG_LOGIC | DBG_BANK | DBG_VIDEO | DBG_SER | DBG_SND
	SystemStub *stub = SystemStub_SDL_create();
	Engine *e = new Engine(stub, dataPath, savePath);
	e->run();
	delete e;
	delete stub;
	return 0;
}
