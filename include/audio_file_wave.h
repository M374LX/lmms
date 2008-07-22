/*
 * audio_file_wave.h - Audio-device which encodes wave-stream and writes it
 *                     into an WAVE-file. This is used for song-export.
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


#ifndef _AUDIO_FILE_WAVE_H
#define _AUDIO_FILE_WAVE_H


#include "lmmsconfig.h"
#include "audio_file_device.h"

#include <sndfile.h>


class audioFileWave : public audioFileDevice
{
public:
	audioFileWave( const sample_rate_t _sample_rate,
			const ch_cnt_t _channels,
			bool & _success_ful,
			const QString & _file,
			const bool _use_vbr,
			const bitrate_t _nom_bitrate,
			const bitrate_t _min_bitrate,
			const bitrate_t _max_bitrate,
			const int _depth,
			mixer * _mixer );
	virtual ~audioFileWave();

	static audioFileDevice * getInst( const sample_rate_t _sample_rate,
						const ch_cnt_t _channels,
						bool & _success_ful,
						const QString & _file,
						const bool _use_vbr,
						const bitrate_t _nom_bitrate,
						const bitrate_t _min_bitrate,
						const bitrate_t _max_bitrate,
						const int _depth,
						mixer * _mixer )
	{
		return( new audioFileWave( _sample_rate, _channels,
						_success_ful, _file, _use_vbr,
						_nom_bitrate, _min_bitrate,
							_max_bitrate, _depth,
							_mixer ) );
	}


private:
	virtual void writeBuffer( const surroundSampleFrame * _ab,
						const fpp_t _frames,
						float _master_gain );

	bool startEncoding( void );
	void finishEncoding( void );


	SF_INFO m_si;
	SNDFILE * m_sf;

} ;


#endif
