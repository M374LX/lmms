/*
 * bb_track_container.cpp - model-component of BB-Editor
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


#include "bb_track_container.h"
#include "bb_track.h"
#include "combobox.h"
#include "embed.h"
#include "engine.h"
#include "song.h"



bbTrackContainer::bbTrackContainer( void ) :
	trackContainer(),
	m_bbComboBoxModel( this )
{
	connect( &m_bbComboBoxModel, SIGNAL( dataChanged() ),
			this, SLOT( currentBBChanged() ) );
	// we *always* want to receive updates even in case BB actually did
	// not change upon setCurrentBB()-call
	connect( &m_bbComboBoxModel, SIGNAL( dataUnchanged() ),
			this, SLOT( currentBBChanged() ) );
}




bbTrackContainer::~bbTrackContainer()
{
}




bool bbTrackContainer::play( midiTime _start, fpp_t _frames,
							f_cnt_t _offset,
							Sint16 _tco_num )
{
	bool played_a_note = FALSE;
	if( lengthOfBB( _tco_num ) <= 0 )
	{
		return( FALSE );
	}

	_start = _start % ( lengthOfBB( _tco_num ) * midiTime::ticksPerTact() );

	trackList tl = tracks();
	for( trackList::iterator it = tl.begin(); it != tl.end(); ++it )
	{
		if( ( *it )->play( _start, _frames, _offset, _tco_num ) )
		{
			played_a_note = TRUE;
		}
	}

	return( played_a_note );
}




void bbTrackContainer::updateAfterTrackAdd( void )
{
	// make sure, new track(s) have TCOs for every beat/bassline
	for( int i = 0; i < tMax<int>( 1, numOfBBs() ); ++i )
	{
		createTCOsForBB( i );
	}
}




tact bbTrackContainer::lengthOfBB( int _bb )
{
	midiTime max_length = midiTime::ticksPerTact();

	const trackList & tl = tracks();
	for( trackList::const_iterator it = tl.begin(); it != tl.end(); ++it )
	{
		max_length = tMax( max_length,
					( *it )->getTCO( _bb )->length() );
	}

	return( max_length.nextFullTact() );
}




int bbTrackContainer::numOfBBs( void ) const
{
	return( engine::getSong()->countTracks( track::BBTrack ) );
}




void bbTrackContainer::removeBB( int _bb )
{
	trackList tl = tracks();
	for( trackList::iterator it = tl.begin(); it != tl.end(); ++it )
	{
		delete ( *it )->getTCO( _bb );
		( *it )->removeTact( _bb * DefaultTicksPerTact );
	}
	if( _bb <= currentBB() )
	{
		setCurrentBB( tMax( currentBB() - 1, 0 ) );
	}
}




void bbTrackContainer::swapBB( int _bb1, int _bb2 )
{
	trackList tl = tracks();
	for( trackList::iterator it = tl.begin(); it != tl.end(); ++it )
	{
		( *it )->swapPositionOfTCOs( _bb1, _bb2 );
	}
	updateComboBox();
}




void bbTrackContainer::updateBBTrack( trackContentObject * _tco )
{
	bbTrack * t = bbTrack::findBBTrack( _tco->startPosition() /
							DefaultTicksPerTact );
	if( t != NULL )
	{
		t->dataChanged();
	}
}




void bbTrackContainer::fixIncorrectPositions( void )
{
	trackList tl = tracks();
	for( trackList::iterator it = tl.begin(); it != tl.end(); ++it )
	{
		for( int i = 0; i < numOfBBs(); ++i )
		{
			( *it )->getTCO( i )->movePosition( midiTime( i, 0 ) );
		}
	}
}




void bbTrackContainer::play( void )
{
	if( engine::getSong()->isPlaying() )
	{
		if( engine::getSong()->playMode() != song::Mode_PlayBB )
		{
			engine::getSong()->stop();
			engine::getSong()->playBB();
		}
		else
		{
			engine::getSong()->pause();
		}
	}
	else if( engine::getSong()->isPaused() )
	{
		engine::getSong()->resumeFromPause();
	}
	else
	{
		engine::getSong()->playBB();
	}

}




void bbTrackContainer::stop( void )
{
	engine::getSong()->stop();
}




void bbTrackContainer::updateComboBox( void )
{
	const int cur_bb = currentBB();

	m_bbComboBoxModel.clear();

	for( int i = 0; i < numOfBBs(); ++i )
	{
		bbTrack * bbt = bbTrack::findBBTrack( i );
		m_bbComboBoxModel.addItem( bbt->name(),
					new pixmapLoader( bbt->icon() ) );
	}
	setCurrentBB( cur_bb );
}




void bbTrackContainer::currentBBChanged( void )
{
	// first make sure, all channels have a TCO at current BB
	createTCOsForBB( currentBB() );

	// now update all track-labels (the current one has to become white,
	// the others gray)
	trackList tl = engine::getSong()->tracks();
	for( trackList::iterator it = tl.begin(); it != tl.end(); ++it )
	{
		if( ( *it )->type() == track::BBTrack )
		{
			( *it )->dataChanged();
		}
	}
}




void bbTrackContainer::createTCOsForBB( int _bb )
{
	if( numOfBBs() == 0 || engine::getSong()->isLoadingProject() )
	{
		return;
	}

	trackList tl = tracks();
	for( int i = 0; i < tl.size(); ++i )
	{
		while( tl[i]->numOfTCOs() < _bb + 1 )
		{
			midiTime position = midiTime( tl[i]->numOfTCOs(), 0 );
			trackContentObject * tco = tl[i]->createTCO( position );
			tco->movePosition( position );
			tco->changeLength( midiTime( 1, 0 ) );
		}
	}
}



#include "moc_bb_track_container.cxx"

