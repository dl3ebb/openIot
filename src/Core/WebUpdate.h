#include "openIot.h"
#ifndef UPDATE_H
#define UPDATE_H

void doUpdate (void *parameter);

class WebUpdate {
	

	public:
		void begin ();
		void updateCheck ();
		bool checkForUpdate ();
		int updateVersion = 0;
		bool performUpdate (bool updateFlash);
};
#endif