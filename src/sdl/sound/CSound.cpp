/*
 * CSound.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#include "../../keen.h"
#include "../../include/fileio/lzexe.h"
#include "CSound.h"
#include "../../include/fileio.h"
#include "../../CLogFile.h"
#include "../../hqp/CMusic.h"
#include "../../vorticon/sounds.h"

#define SAFE_DELETE_ARRAY(x) if(x) delete[] x; x=NULL

#define BLITBUF_XSIZE  320 		// Used only for balance in stereo mode

// define a callback function we can work with
void CCallback(void *unused, Uint8 *stream, int len)
{
    // let it call a method on my (singleton) sound object
    g_pSound->callback(unused, stream, len);
}


CSound::CSound() {
	m_active = false;
	m_volume = 0;
	m_mixing_channels = 0;
	m_soundchannel = NULL;
	m_soundslot = NULL;
	m_MixedForm = NULL;
	AudioSpec.channels = 2; // Stereo Sound
	AudioSpec.format = AUDIO_U8; // 8-bit sound
	AudioSpec.freq = 48000; // high quality
}

CSound::~CSound() {
	destroy();
	if (m_soundslot) { delete[] m_soundslot; m_soundslot = NULL; }
	if (m_soundchannel) { delete[] m_soundchannel; m_soundchannel = NULL; }
}

bool CSound::init(void)
{
  char name[MAX_STRING_LENGTH];

  // now start up the SDL sound system
  AudioSpec.silence = 0;
  AudioSpec.samples = 1024;
  AudioSpec.callback = CCallback;
  AudioSpec.userdata = NULL;

  /* Initialize fillerup() variables */
  if( SDL_OpenAudio(&AudioSpec, NULL) < 0 )
  {
	  g_pLogFile->ftextOut("SoundDrv_Start(): The Sound settings don't work! Going into Failsafemode!<br>");

	  AudioSpec.channels = 1;
	  AudioSpec.freq = 11000;

	  if( SDL_OpenAudio(&AudioSpec, NULL) < 0 )
	  {
		  g_pLogFile->ftextOut("SoundDrv_Start(): Couldn't open audio: %s<br>", SDL_GetError());
		  g_pLogFile->ftextOut("Sound will be disabled.<br>");
		  AudioSpec.channels = 0;
		  AudioSpec.format = 0;
		  AudioSpec.freq = 0;
		  m_active = false;
		  return false;
	  }
  }

  m_MixedForm = new Uint8[AudioSpec.size];

  g_pLogFile->ftextOut("SDL_AudioSpec:<br>");
  g_pLogFile->ftextOut("  freq: %d<br>", AudioSpec.freq);
  g_pLogFile->ftextOut("  channels: %d<br>", AudioSpec.channels);
  g_pLogFile->ftextOut("  audio buffer size: %d<br>", AudioSpec.size);
  g_pLogFile->ftextOut("Using audio driver: %s<br>", SDL_AudioDriverName(name, 32));

  m_mixing_channels = 7;

  switch(m_mixing_channels)
  {
  case 15: m_mixing_channels_base = 4; break;
  case 7: m_mixing_channels_base = 3; break;
  case 3: m_mixing_channels_base = 2;
  default: break;
  }

  m_volume = 2; // Max Value!;

  if(m_soundchannel) delete[] m_soundchannel; m_soundchannel = NULL;
  m_soundchannel = new CSoundChannel[m_mixing_channels];

  for(unsigned short i=0 ; i < m_mixing_channels ; i++)
	  m_soundchannel[i].setFrequencyCorrection(AudioSpec.freq);

  SDL_PauseAudio(0);

  g_pLogFile->ftextOut("Sound System: SDL sound system initialized.<br>");
  m_active = true;

  return true;
}

void CSound::destroy(void)
{
   stopAllSounds();

   if (m_active)
   {
     SDL_PauseAudio(1);
     SDL_CloseAudio();
     m_active = false;
     m_mixing_channels = 0;
   }
   SAFE_DELETE_ARRAY(m_MixedForm);
   SAFE_DELETE_ARRAY(m_soundchannel);
   SAFE_DELETE_ARRAY(m_soundslot);

   g_pLogFile->ftextOut("SoundDrv_Stop(): shut down.<br>");
}


// stops all currently playing sounds
void CSound::stopAllSounds(void)
{
	for(unsigned int chnl=0;chnl<m_mixing_channels;chnl++)
		m_soundchannel[chnl].stopSound();
}

// pauses any currently playing sounds
void CSound::pauseSound(void)
{
   if (m_active) SDL_PauseAudio(1);
}

// resumes playing a previously paused sound
void CSound::resumeSounds(void)
{
   if (m_active) SDL_PauseAudio(0);
}

// returns true if sound snd is currently playing
bool CSound::isPlaying(int snd)
{
	for(int i=0;i<m_mixing_channels;i++)
	{
		if (m_soundchannel[i].isPlaying())
			if (m_soundchannel[i].getCurrentsound() == snd)
				return true;
	}
	return false;
}

// if sound snd is currently playing, stops it immediately
void CSound::stopSound(int snd)
{
	for(unsigned int chnl=0;chnl<m_mixing_channels;chnl++)
		if (m_soundchannel[chnl].isPlaying())
			if (m_soundchannel[chnl].getCurrentsound()==snd)
				m_soundchannel[chnl].stopSound();
}

// returns true if a sound is currently playing in PLAY_FORCE mode
bool CSound::forcedisPlaying(void)
{
	for(unsigned int i=0;i<m_mixing_channels;i++)
		if(m_soundchannel[i].isForcedPlaying())
			return true;

	return false;
}

void CSound::callback(void *unused, Uint8 *stream, int len)
{
    unsigned short i;

    if (g_pMusicPlayer->playing() == PLAY_MODE_PLAY)
    {
       	SDL_MixAudio(stream, g_pMusicPlayer->passBuffer(len), len, SDL_MIX_MAXVOLUME);
    }

    for( i=0 ; i < m_mixing_channels ; i++ )
   	{
   		m_soundchannel[i].readWaveform(m_MixedForm, len, AudioSpec.channels, AudioSpec.freq);
   		SDL_MixAudio(stream, m_MixedForm, len, SDL_MIX_MAXVOLUME);
    }
}

// if priorities allow, plays the sound "snd".
// nonzero return value indicates a higher priority sound is playing.
void CSound::playSound(int snd, char mode)
{
	playStereosound(snd, mode, 0);
}

void CSound::playStereofromCoord(int snd, char mode, unsigned int xcoordinate)
{
    if(AudioSpec.channels == 2)
    {
    	int bal;

    	bal = ((short)(xcoordinate) - (BLITBUF_XSIZE>>1));	// Faster calculation of balance transformation

    	if(bal < -127)
    		bal = -127;
    	else if(bal > 127)
    		bal = 127;

    	playStereosound(snd, mode, bal);
	}
    else
    	playSound(snd, mode);
}

void CSound::playStereosound(int snd, char mode, short balance)
{
	if(!(m_mixing_channels && m_active)) return;

	short chnl = 0;
	int i;

	if (mode==PLAY_NORESTART)
	{
		if (isPlaying(snd))
			return;
	}

	// if a forced sound is playing then let it play
	if (forcedisPlaying()) return;

	// stop all other sounds if this sound has maximum priority
	if (m_soundslot[snd].getPriority()==255 || mode==PLAY_FORCE)
	{
		stopAllSounds();
	}

  if (snd==SOUND_KEEN_FALL)
  {  // only play KEEN_FALL if no other sounds are playing
    for(chnl=0;chnl<m_mixing_channels;chnl++)
    {
      if (m_soundchannel[chnl].isPlaying())
      {
        if (m_soundslot[m_soundchannel[chnl].getCurrentsound()].getPriority() > m_soundslot[snd].getPriority())
        {
          return;
        }
      }
    }
    chnl = 0;
    goto playsound;
  }

  // first try to find an empty channel
  for(chnl=0;chnl<m_mixing_channels;chnl++)
  {
    if (!m_soundchannel[chnl].isPlaying())
    {
      goto startsound;
    }
  }
  // if all channels are full see if we have higher
  // priority than one of the sounds already playing.
  for(chnl=0;chnl<m_mixing_channels;chnl++)
  {
    if (m_soundslot[m_soundchannel[chnl].getCurrentsound()].getPriority() <= m_soundslot[snd].getPriority())
    {
      goto startsound;
    }
  }
  // can't play sound right now.
  return;

startsound: ;
  // don't play more than once instance
  // of the same sound in a seperate channel--
  // instead restart the currently playing sound
  for(i=0;i<m_mixing_channels;i++)
  {
    if (m_soundchannel[i].getCurrentsound() == snd)
    {
      chnl = i;
      break;
    }
  }

playsound: ;
  // stop SOUND_KEEN_FALL if playing
  if (isPlaying(SOUND_KEEN_FALL))
	  stopSound(SOUND_KEEN_FALL);

  if(AudioSpec.channels == 2)
	  m_soundchannel[chnl].setBalance(balance);

  m_soundchannel[chnl].enableHighQuality(m_soundslot[snd].isHighQuality());
  m_soundchannel[chnl].setupSound((unsigned short)snd, 0, true, WAVE_IN, 0, (mode==PLAY_FORCE) ? true : false );
}

char CSound::loadSoundData(unsigned short Episode, char *DataDirectory)
{
  if(!m_active) return false;

  char *path;
  int ok;
  char soundfile[80];
  char buf[256];

  if(m_soundslot) delete[] m_soundslot;
  m_soundslot = new CSoundSlot[MAX_SOUNDS];

  for(int i=0 ; i<MAX_SOUNDS ; i++)
  {
	  m_soundslot[i].setpAudioSpec(&AudioSpec);
  }

  path = DataDirectory;

  g_pLogFile->ftextOut("sound_load_all(): loading all sounds...<br>");

  char buffer[256];

  formatPathString(buffer,path);

  sprintf(soundfile, "%ssounds.ck%d", buffer,Episode);

  FILE *p_file;

  if( ( p_file = fopen(soundfile,"rb") ) == NULL )
  {
	formatPathString(buffer,path);

	sprintf(buf,"keen%d.exe",Episode);
	g_pLogFile->ftextOut("sound_load_all(): \"%s\" was not found in the data directory. Looking for \"%s\" in \"%s\" and trying to extract this file<br>", soundfile, buf, buffer);
	extractOfExeFile(buffer, Episode);
  }
  else
	 fclose(p_file);

  ok  = m_soundslot[SOUND_KEEN_WALK].loadSound(soundfile, "KEENWALKSND", SOUND_KEEN_WALK);
  ok |= m_soundslot[SOUND_KEEN_WALK2].loadSound(soundfile, "KEENWLK2SND", SOUND_KEEN_WALK2);
  ok |= m_soundslot[SOUND_KEEN_JUMP].loadSound(soundfile, "KEENJUMPSND", SOUND_KEEN_JUMP);
  ok |= m_soundslot[SOUND_KEEN_POGO].loadSound(soundfile, "KEENPOGOSND", SOUND_KEEN_POGO);
  ok |= m_soundslot[SOUND_KEEN_LAND].loadSound(soundfile, "KEENLANDSND", SOUND_KEEN_LAND);
  ok |= m_soundslot[SOUND_KEEN_BLOK].loadSound(soundfile, "KEENBLOKSND", SOUND_KEEN_BLOK);
  ok |= m_soundslot[SOUND_KEEN_DIE].loadSound(soundfile, "KEENDIESND", SOUND_KEEN_DIE);
  ok |= m_soundslot[SOUND_KEEN_FALL].loadSound(soundfile, "PLUMMETSND", SOUND_KEEN_FALL);
  ok |= m_soundslot[SOUND_KEEN_BUMPHEAD].loadSound(soundfile, "BUMPHEADSND", SOUND_KEEN_BUMPHEAD);
  ok |= m_soundslot[SOUND_ENTER_LEVEL].loadSound(soundfile, "WLDENTERSND", SOUND_ENTER_LEVEL);
  ok |= m_soundslot[SOUND_KEENSLEFT].loadSound(soundfile, "keensleft", SOUND_KEENSLEFT);

  ok |= m_soundslot[SOUND_KEEN_FIRE].loadSound(soundfile, "KEENFIRESND", SOUND_KEEN_FIRE);
  ok |= m_soundslot[SOUND_GUN_CLICK].loadSound(soundfile, "GUNCLICK", SOUND_GUN_CLICK);
  ok |= m_soundslot[SOUND_SHOT_HIT].loadSound(soundfile, "SHOTHIT", SOUND_SHOT_HIT);

  ok |= m_soundslot[SOUND_GET_ITEM].loadSound(soundfile, "GOTITEMSND", SOUND_GET_ITEM);
  ok |= m_soundslot[SOUND_GET_BONUS].loadSound(soundfile, "GOTBONUSSND", SOUND_GET_BONUS);
  ok |= m_soundslot[SOUND_GET_PART].loadSound(soundfile, "GOTPARTSND", SOUND_GET_PART);
  ok |= m_soundslot[SOUND_LEVEL_DONE].loadSound(soundfile, "LVLDONESND", SOUND_LEVEL_DONE);
  ok |= m_soundslot[SOUND_GAME_OVER].loadSound(soundfile, "GAMEOVERSND", SOUND_GAME_OVER);
  ok |= m_soundslot[SOUND_TELEPORT].loadSound(soundfile, "TELEPORTSND", SOUND_TELEPORT);
  ok |= m_soundslot[SOUND_EXTRA_LIFE].loadSound(soundfile, "EXTRAMANSND", SOUND_EXTRA_LIFE);
  ok |= m_soundslot[SOUND_CHUNKSMASH].loadSound(soundfile, "CHUNKSMASH", SOUND_CHUNKSMASH);
  ok |= m_soundslot[SOUND_GOINDOOR].loadSound(soundfile, "GOINDOORSND", SOUND_GOINDOOR);
  ok |= m_soundslot[SOUND_GET_CARD].loadSound(soundfile, "GETCARDSND", SOUND_GET_CARD);
  ok |= m_soundslot[SOUND_USE_KEY].loadSound(soundfile, "USEKEYSND", SOUND_USE_KEY);
  ok |= m_soundslot[SOUND_SWITCH_TOGGLE].loadSound(soundfile, "CLICKSND", SOUND_SWITCH_TOGGLE);
  ok |= m_soundslot[SOUND_DOOR_OPEN].loadSound(soundfile, "DOOROPENSND", SOUND_DOOR_OPEN);

  ok |= m_soundslot[SOUND_YORP_BUMP].loadSound(soundfile, "YORPBUMPSND", SOUND_YORP_BUMP);
  ok |= m_soundslot[SOUND_YORP_STUN].loadSound(soundfile, "YORPBOPSND", SOUND_YORP_STUN);
  ok |= m_soundslot[SOUND_YORP_DIE].loadSound(soundfile, "YORPSCREAM", SOUND_YORP_DIE);
  ok |= m_soundslot[SOUND_GARG_DIE].loadSound(soundfile, "GARGSCREAM", SOUND_GARG_DIE);
  ok |= m_soundslot[SOUND_VORT_DIE].loadSound(soundfile, "vortscream", SOUND_VORT_DIE);
  ok |= m_soundslot[SOUND_TANK_FIRE].loadSound(soundfile, "TANKFIRE", SOUND_TANK_FIRE);

  if (Episode == 2)
  {
    ok |= m_soundslot[33].loadSound(soundfile, "EARTHPOW", SOUND_EARTHPOW);
  }
  else if (Episode == 3)
  {
    ok |= m_soundslot[SOUND_MEEP].loadSound(soundfile, "MEEP", SOUND_MEEP);
    ok |= m_soundslot[SOUND_ANKH].loadSound(soundfile, "ANKH", SOUND_ANKH);
    ok |= m_soundslot[SOUND_MORTIMER].loadSound(soundfile, "MORTIMER", SOUND_MORTIMER);
    ok |= m_soundslot[SOUND_FOOTSLAM].loadSound(soundfile, "FOOTSLAM", SOUND_FOOTSLAM);
  }

  for( unsigned short i=0 ; i<m_mixing_channels ; i++ )
	  m_soundchannel[i].setSoundSlotPtr(m_soundslot);

  return ok;
}


/*  char sound_extraction_of_exe_files(char *inputfname, char *outputfname)

    Utility to extract the embedded sounds from Commander Keen
    Episode 2 (keen2.exe) and Episode 3 (keen3.exe)

    Implemented by Gerhard Stein  <gerstrong@gmail.com> (2008)
    Copyright (C) 2007 Hans de Goede  <j.w.r.degoede@hhs.nl>

    Many thanks to Mitugu(Kou) Kurizono for the decompression algorithm.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

char CSound::extractOfExeFile(char *inputpath, int episode)
{
	  const char *outputfname;
	  int bit_count;
	  FILE *fin, *fout;
	  unsigned char buf[131072];
	  short offset;
	  int pos, repeat, sounds_start, sounds_end, ret = 0;
	  char buffer[MAX_STRING_LENGTH];
	  char inputfname[MAX_STRING_LENGTH];

	  pos = 0;
	  bit_count = 0;
	  memset(buffer,0, MAX_STRING_LENGTH*sizeof(char));
	  memset(inputfname,0, MAX_STRING_LENGTH*sizeof(char));

	  if (episode == 2)
	  {
	    outputfname = "sounds.ck2";
	    sounds_start  = 0x12730;
	    sounds_end    = 0x14BDA;
	  }
	  else if (episode == 3)
	  {
	    outputfname = "sounds.ck3";
	    sounds_start  = 0x13A70;
	    sounds_end    = 0x164D4;
	  }
	  else
	  {
		  g_pLogFile->ftextOut("Error: Unknown keen executable name: %s<br>", inputfname);
	    return 1;
	  }

	  strcpy(buffer,inputpath);
	  sprintf(inputfname,"%skeen%d.exe", inputpath, episode);

	  fin = fopen(inputfname, "rb");

	  if (!fin)
	  {
		  g_pLogFile->ftextOut( "Error opening input file %s: ", inputfname);
	    perror(NULL);
	    return 2;
	  }
	  else
	  {
		  sprintf(buffer,"%s%s", inputpath, outputfname);

		  fout = fopen(buffer, "wb");
		  if (!fout)
		  {
			  g_pLogFile->ftextOut( "Error opening output file %s: ", outputfname);
			  perror(NULL);
			  fclose(fin);
		  }

		  /* skip header */
		  fseek(fin, 32, SEEK_SET);

		  while (1)
		  {
			  if (ferror(fin))
			  {
				  g_pLogFile->ftextOut( "Error reading from input file %s:", inputfname);
				  perror(NULL);
				  fclose(fin);
				  fclose(fout);
				  return 1;
			  }

			  if (get_bit(&bit_count, &fin))
			  {
				  buf[pos++] = getc(fin);
			  }
			  else
			  {
				  if (get_bit(&bit_count, &fin))
				  {
					  unsigned char tmp[2];
					  fread(tmp, 1, 2, fin);
					  repeat = (tmp[1] & 0x07);

					  offset = ((tmp[1] & ~0x07) << 5) | tmp[0] | 0xE000;

					  if (repeat == 0)
					  {
						  repeat = getc (fin);


						  if (repeat == 0)
							  break;
						  else if (repeat == 1)
							  continue;
						  else
							  repeat++;
					  }
					  else
						  repeat += 2;
				  }
				  else
				  {
					  repeat = ((get_bit(&bit_count, &fin) << 1) | get_bit(&bit_count, &fin)) + 2;
					  offset = getc(fin) | 0xFF00;
				  }

				  while (repeat > 0)
				  {
					  buf[pos] = buf[pos + offset];
					  pos++;
					  repeat--;
				  }
			}
		  }
		  g_pLogFile->ftextOut("Decompression (unlzexe) of %s done<br>", inputfname);

		  if (strcmp((char *)(&buf[sounds_start]), "SND"))
		  {
			  g_pLogFile->ftextOut( "Error: Beginning of sound data not found at expected offset<br>");
			ret = 1;
		  }
		  else if ((int)fwrite(&buf[sounds_start], 1, sounds_end - sounds_start, fout) !=
			  (sounds_end - sounds_start))
		  {
			  g_pLogFile->ftextOut( "error writing to output file %s: ", outputfname);
			perror(NULL);
			ret = 1;
		  }
		  g_pLogFile->ftextOut("%s has been successfully written<br>", outputfname);

		  fclose(fin);
		  fclose(fout);

	  }

	  return ret;
}
void CSound::setSoundmode(int freq, bool stereo)
{
	if(stereo)
		AudioSpec.channels = 2;
	else
		AudioSpec.channels = 1;

	switch(freq)
	{
	case 0: break; // means that the actual frequency stays untouched
	case 11000:
	case 22050:
	case 44100:
	case 48000:
		AudioSpec.freq = freq;
		break;
	default:
		AudioSpec.freq = 48000;
	}
}

