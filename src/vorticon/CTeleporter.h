/*
 * CTeleporter.h
 *
 *  Created on: 13.10.2009
 *      Author: gerstrong
 */

#ifndef CTELEPORTER_H_
#define CTELEPORTER_H_

class CTeleporter {
public:
	CTeleporter();
	virtual ~CTeleporter();

	bool readTeleporterInfo();
	void teleportPlayer();
};

#endif /* CTELEPORTER_H_ */
