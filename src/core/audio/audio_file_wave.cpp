#ifndef SINGLE_SOURCE_COMPILE

/*
 * audio_file_wave.cpp - audio-device which encodes wave-stream and writes it
 *                       into a WAVE-file. This is used for song-export.
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



#include "audio_file_wave.h"
#include "endian_handling.h"

#include <cstring>


audioFileWave::audioFileWave( const sample_rate_t _sample_rate,
				const ch_cnt_t _channels, bool & _success_ful,
				const QString & _file,
				const bool _use_vbr,
				const bitrate_t _nom_bitrate,
				const bitrate_t _min_bitrate,
				const bitrate_t _max_bitrate,
				const int _depth,
				mixer * _mixer ) :
	audioFileDevice( _sample_rate, _channels, _file, _use_vbr,
			_nom_bitrate, _min_bitrate, _max_bitrate,
								_depth, _mixer )
{
	_success_ful = startEncoding();
}




audioFileWave::~audioFileWave()
{
	finishEncoding();
}




bool audioFileWave::startEncoding( void )
{
	m_si.samplerate = sampleRate();
	m_si.channels = channels();
	m_si.frames = getMixer()->framesPerPeriod();
	m_si.sections = 1;
	m_si.seekable = 0;

	switch( depth() )
	{
		case 32: m_si.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT; break;
		case 16:
		default: m_si.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; break;
	}
	m_sf = sf_open( outputFile().toUtf8().constData(), SFM_WRITE, &m_si );
	return( TRUE );
}




void audioFileWave::writeBuffer( const surroundSampleFrame * _ab,
						const fpp_t _frames,
						const float _master_gain )
{
	if( depth() == 32 )
	{
		float *  buf = new float[_frames*channels()];
		for( fpp_t frame = 0; frame < _frames; ++frame )
		{
			for( ch_cnt_t chnl = 0; chnl < channels(); ++chnl )
			{
				buf[frame*channels()+chnl] = _ab[frame][chnl] *
								_master_gain;
			}
		}
		sf_writef_float( m_sf, buf, _frames );
		delete[] buf;
	}
	else
	{
		int_sample_t * buf = new int_sample_t[_frames * channels()];
		convertToS16( _ab, _frames, _master_gain, buf,
							!isLittleEndian() );

		sf_writef_short( m_sf, buf, _frames );
		delete[] buf;
	}
}




void audioFileWave::finishEncoding( void )
{
	sf_close( m_sf );
}


#endif
