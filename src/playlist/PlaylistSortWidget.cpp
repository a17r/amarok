/****************************************************************************************
 * Copyright (c) 2009 Teo Mrnjavac <teo.mrnjavac@gmail.com>                             *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Pulic License for more details.              *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#include "PlaylistSortWidget.h"

#include "Debug.h"

#include <KPushButton>

namespace Playlist
{

SortWidget::SortWidget( QWidget *parent ) : QWidget( parent )
{
    DEBUG_BLOCK
    QHBoxLayout *mainLayout = new QHBoxLayout( this );
    setLayout( mainLayout );
    m_comboLayout = new QHBoxLayout( this );
    mainLayout->addLayout( m_comboLayout );
    m_sortableCategories.append( internalColumnNames );
    for( QStringList::iterator i = m_sortableCategories.begin(); i!=m_sortableCategories.end(); )
    {
        if( *i == QString( "Placeholder" ) || *i == QString( "Bpm" )
            || *i == QString( "Cover image" ) || *i == QString( "Divider" )
            || *i == QString( "Last played" ) || *i == QString( "Mood" )
            || *i == QString( "Source" ) || *i == QString( "SourceEmblem" )
            || *i == QString( "Title (with track number)" ) || *i == QString( "Type" ) )
            i = m_sortableCategories.erase( i );
        else
            ++i;
    }

    KPushButton *btnPushLevel = new KPushButton( KIcon( "list-add" ), "", this );
    mainLayout->addWidget( btnPushLevel );
    btnPushLevel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    btnPushLevel->resize( btnPushLevel->height(), btnPushLevel->height() );
    KPushButton *btnPopLevel = new KPushButton( KIcon( "edit-delete" ), "", this );
    mainLayout->addWidget( btnPopLevel );
    btnPopLevel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    btnPopLevel->resize( btnPopLevel->height(), btnPopLevel->height() );

    mainLayout->addStretch();
    KPushButton *btnSort = new KPushButton( "Just sort it!", this );
    mainLayout->addWidget( btnSort );

    connect(btnPushLevel, SIGNAL( clicked() ), this, SLOT( pushLevel() ) );
    connect(btnPopLevel, SIGNAL( clicked() ), this, SLOT( popLevel() ) );
    connect(btnSort, SIGNAL( clicked() ), this, SLOT( applySortingScheme() ) );
}

void
SortWidget::applySortingScheme()
{
    DEBUG_BLOCK
    SortScheme scheme = SortScheme();
    for( QList< KComboBox * >::const_iterator i = m_comboList.begin(); i!=m_comboList.end(); ++i )
    {
        scheme.addLevel( SortLevel( internalColumnNames.indexOf( (*i)->currentText() ), Qt::DescendingOrder ) );
    }
    SortProxy::instance()->updateSortMap( scheme );
}

void
SortWidget::pushLevel()
{
    m_comboList.append( new KComboBox( this ) );
    m_comboLayout->addWidget( m_comboList.back() );
    m_comboList.back()->addItems( m_sortableCategories );
}

void
SortWidget::popLevel()
{
    if( !m_comboList.isEmpty() )
    {
        m_comboLayout->removeWidget( m_comboList.back() );
        delete m_comboList.takeLast();
    }
}

}   //namespace Playlist
