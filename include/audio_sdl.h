/*
 * audio_sdl.h - device-class that performs PCM-output via SDL
 *
 * Copyright (c) 2004-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * 
 * This file is part of Linux MultiMedia Studio - http://lmms.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */


#ifndef _AUDIO_SDL_H
#define _AUDIO_SDL_H

#include "lmmsconfig.h"

#ifdef LMMS_HAVE_SDL

#include <SDL.h>
#include <SDL_audio.h>

#include "audio_device.h"


class QLineEdit;


class audioSDL : public audioDevice
{
public:
	audioSDL( bool & _success_ful, mixer * _mixer );
	virtual ~audioSDL();

	inline static QString name( void )
	{
		return( QT_TRANSLATE_NOOP( "setupWidget",
					"SDL (Simple DirectMedia Layer)" ) );
	}


	class setupWidget : public audioDevice::setupWidget
	{
	public:
		setupWidget( QWidget * _parent );
		virtual ~setupWidget();

		virtual void saveSettings( void );

	private:
		QLineEdit * m_device;

	} ;


private:
	virtual void startProcessing( void );
	virtual void stopProcessing( void );
	virtual void applyQualitySettings( void );

	static void sdlAudioCallback( void * _udata, Uint8 * _buf, int _len );
	void sdlAudioCallback( Uint8 * _buf, int _len );

	SDL_AudioSpec m_audioHandle;

	surroundSampleFrame * m_outBuf;
	Uint8 * m_convertedBuf;
	int m_convertedBuf_pos;
	int m_convertedBuf_size;

	bool m_convertEndian;

	bool m_stopped;
	QSemaphore m_stop_semaphore;

} ;

#endif

#endif
