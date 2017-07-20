/*
 * Configurator.h
 *
 *  Created on: 17.06.2012
 *      Author: gerstrong
 *
 *  This header file helps to set Makro Definitions for proper devices.
 *  Please put your configurations in here if you want to specialize your version of CG.
 *  For example Android devices should not have Multiplayer modus and be able to change resolution.
 */

#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_


#if defined(ANDROID)
	#define EMBEDDED
	#undef USE_OPENGL
#endif

#if defined(EMBEDDED) || TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
	#define SINGLEPLAYER
#endif


#endif /* CONFIGURATOR_H_ */
