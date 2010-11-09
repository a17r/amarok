/****************************************************************************************
 * Copyright (c) 2008 Seb Ruiz <ruiz@kde.org>                                           *
 * Copyright (c) 2010 Rick W. Chen <stuffcorpse@archlinux.us>                           *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#include "CoverViewDialog.h"

#include "widgets/PixmapViewer.h"

#include <KApplication>
#include <KDialog>
#include <KLocalizedString>
#include <KWindowSystem>

#include <QDesktopWidget>
#include <QHBoxLayout>

CoverViewDialog::CoverViewDialog( Meta::AlbumPtr album, QWidget *parent )
    : QDialog( parent )
    , m_title( i18n( "Cover View" ) )
    , m_size( album->image().size() )
    , m_zoom( 100 )
{
    init();
    updateCaption();
    createViewer( album->image(), parent );
}

CoverViewDialog::CoverViewDialog( const QPixmap &pixmap, QWidget *parent )
    : QDialog( parent )
    , m_title( i18n( "Cover View" ) )
    , m_size( pixmap.size() )
    , m_zoom( 100 )
{
    init();
    updateCaption();
    createViewer( pixmap, parent );
}

void
CoverViewDialog::updateCaption()
{
    QString width   = QString::number( m_size.width() );
    QString height  = QString::number( m_size.height() );
    QString zoom    = QString::number( m_zoom );
    QString size    = QString( "%1x%2" ).arg( width, height );
    QString caption = QString( "%1 - %2 - %3\%" ).arg( m_title, size, zoom );
    setWindowTitle( KDialog::makeStandardCaption( caption ) );
}

void
CoverViewDialog::zoomFactorChanged( float value )
{
    m_zoom = 100 * value;
    updateCaption();
}

void
CoverViewDialog::init()
{
    setAttribute( Qt::WA_DeleteOnClose );
    kapp->setTopWidget( this );
#ifdef Q_WS_X11
    KWindowSystem::setType( winId(), NET::Utility );
#endif
}

void
CoverViewDialog::createViewer( const QPixmap &pixmap, const QWidget *widget )
{
    int screenNumber = KApplication::desktop()->screenNumber( widget );
    PixmapViewer *pixmapViewer = new PixmapViewer( this, pixmap, screenNumber );
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->addWidget( pixmapViewer );
    layout->setSizeConstraint( QLayout::SetFixedSize );
    layout->setContentsMargins( 0, 0, 0, 0 );
    connect( pixmapViewer, SIGNAL(zoomFactorChanged(float)), SLOT(zoomFactorChanged(float)) );

    QPoint topLeft = mapFromParent( widget->geometry().center() );
    topLeft -= QPoint( pixmap.width() / 2, pixmap.height() / 2 );
    move( topLeft );
}

#include "CoverViewDialog.moc"
