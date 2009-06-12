/*
   Copyright (C) 2008 Shane King <kde@dontletsstart.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

#ifndef AMAROK_MULTIPLAYABLECAPABILITYIMPL_P_H
#define AMAROK_MULTIPLAYABLECAPABILITYIMPL_P_H

#include "Debug.h"
#include "LastFmServiceConfig.h"
#include "Meta.h"
#include "meta/capabilities/MultiPlayableCapability.h"

#include <lastfm/Track>
#include <lastfm/RadioTuner>

class MultiPlayableCapabilityImpl : public Meta::MultiPlayableCapability, public Meta::Observer
{
    Q_OBJECT
    public:
        MultiPlayableCapabilityImpl( LastFm::Track *track )
            : Meta::MultiPlayableCapability()
            , m_url( track->internalUrl() )
            , m_track( track )
            , m_currentTrack( lastfm::Track() )
        {
            Meta::TrackPtr trackptr( track );
            subscribeTo( trackptr );
            
            connect( track, SIGNAL( skipTrack() ), this, SLOT( skip() ) );
        }

        virtual ~MultiPlayableCapabilityImpl() 
        {}

        virtual void fetchFirst()
        {
            DEBUG_BLOCK
            m_tuner = new lastfm::RadioTuner( lastfm::RadioStation( m_track->uidUrl() ) );
            
            connect( m_tuner, SIGNAL( trackAvailable() ), this, SLOT( slotNewTrackAvailable() ) );
        }
        
        virtual void fetchNext()
        {
            DEBUG_BLOCK
            m_currentTrack = m_tuner->takeNextTrack();
            m_track->setTrackInfo( m_currentTrack );

        }
        
        using Observer::metadataChanged;
        virtual void metadataChanged( Meta::TrackPtr track )
        {
            DEBUG_BLOCK
            const LastFm::TrackPtr ltrack = LastFm::TrackPtr::dynamicCast( track );
            
            if( ltrack.isNull() )
                return;
                
            KUrl url = ltrack->internalUrl();
            if( url.isEmpty() || url != m_url ) // always should let empty url through, since otherwise we swallow an error getting first track
            {
                m_url = url;
                emit playableUrlFetched( url );
            }
        }

    public slots:

        void slotNewTrackAvailable()
        {
            if( m_currentTrack.isNull() ) // we only force a track change at the beginning
            {
                m_currentTrack = m_tuner->takeNextTrack();
                m_track->setTrackInfo( m_currentTrack );
            }
        }
        
        virtual void skip()
        {
            fetchNext();
            // now we force a new signal to be emitted to kick the enginecontroller to moving on
            //KUrl url = m_track->playableUrl();
            //emit playableUrlFetched( url );
        }


    private:
        KUrl m_url;
        LastFm::TrackPtr m_track;

        
        lastfm::Track m_currentTrack;
        lastfm::RadioTuner* m_tuner;
};

#endif
