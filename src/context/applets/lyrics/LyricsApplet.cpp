/***************************************************************************
 * copyright            : (C) 2007 Leo Franchi <lfranchi@gmail.com>        *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "LyricsApplet.h"

#include "amarok.h"
#include "debug.h"
#include "context/Svg.h"

#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
#include <QPoint>

LyricsApplet::LyricsApplet( QObject* parent, const QStringList& args )
    : Plasma::Applet( parent, args )
    , m_header( 0 )
    , m_headerAspectRatio( 0.0 )
    , m_size( QSizeF() )
    , m_lyricsLabel( 0 )
    , m_titleLabel( 0 )
    , m_artistLabel( 0 )
    , m_siteLabel( 0 )
    , m_lyrics( 0 )
    , m_title( 0 )
    , m_artist( 0 )
    , m_site( 0 )
{
    
    setHasConfigurationInterface( false );
    setDrawStandardBackground( true );
    
    dataEngine( "amarok-lyrics" )->connectSource( "lyrics", this );
    
    m_header = new Context::Svg( "widgets/amarok-lyrics", this );
    m_header->setContentType( Context::Svg::SingleImage );
    m_header->resize();
    m_headerAspectRatio = (qreal)m_header->size().height()
        / (qreal)m_header->size().width();
    
    m_lyricsLabel = new QGraphicsSimpleTextItem( this );
    m_titleLabel = new QGraphicsSimpleTextItem( this );
    m_artistLabel = new QGraphicsSimpleTextItem( this );
    m_siteLabel = new QGraphicsSimpleTextItem( this );
    m_lyrics = new QGraphicsTextItem( this );
    m_title = new QGraphicsSimpleTextItem( this );
    m_artist = new QGraphicsSimpleTextItem( this );
    m_site = new QGraphicsSimpleTextItem( this );
    
    QFont labelFont;
    labelFont.setBold( true );
    labelFont.setPointSize( labelFont.pointSize() + 2 );
    m_lyricsLabel->setBrush( Qt::white );
    m_lyricsLabel->setFont( labelFont );
    m_lyricsLabel->setText( i18n( "Lyrics" ) );
    
    m_titleLabel->setBrush( Qt::white );
    m_titleLabel->setFont( labelFont );
    m_titleLabel->setText( i18n( "Title" ) + ":" );
    
    m_artistLabel->setBrush( Qt::white );
    m_artistLabel->setFont( labelFont );
    m_artistLabel->setText( i18n( "Artist" ) + ":" );
    
    m_siteLabel->setBrush( Qt::white );
    m_siteLabel->setFont( labelFont );
    m_siteLabel->setText( i18n( "Site" ) + ":" );
    
    m_lyrics->setDefaultTextColor( Qt::white );
    m_title->setBrush( QBrush( Qt::white ) );
    m_artist->setBrush( QBrush( Qt::white ) );
    m_site->setBrush( QBrush( Qt::white ) );
    
    constraintsUpdated(); 
}

void LyricsApplet::setRect( const QRectF& rect )
{
    DEBUG_BLOCK
        
    setPos( rect.topLeft() );
    m_size = rect.size();
    resize( rect.width() );
}

QSizeF LyricsApplet::contentSize() const
{
//     return m_size;
    qreal height = m_header->size().width() * m_headerAspectRatio;
//     debug() << "returning size( height ):" << height << "+" << m_lyrics->boundingRect().height();
    return QSizeF( m_header->size().width(), 
                   height + m_lyrics->boundingRect().height() );
}

void LyricsApplet::constraintsUpdated()
{
    DEBUG_BLOCK
        
    prepareGeometryChange();
    
    // align items
    m_lyricsLabel->setPos( m_header->elementRect( "title" ).topLeft());
    m_artistLabel->setPos( m_header->elementRect( "titlelabel" ).topLeft() );
    m_titleLabel->setPos( m_header->elementRect( "artistlabel" ).topLeft() );
    m_siteLabel->setPos( m_header->elementRect( "sitelabel" ).topLeft() );
    
    m_lyrics->setPos( m_header->elementRect( "lyrics" ).topLeft() );
    m_title->setPos( m_header->elementRect( "lyricstrackname" ).topLeft() );
    m_artist->setPos( m_header->elementRect( "lyricsartist" ).topLeft() );
    m_site->setPos( m_header->elementRect( "lyricslyricssite" ).topLeft() );
}

void LyricsApplet::updated( const QString& name, const Plasma::DataEngine::Data& data )
{
    DEBUG_BLOCK;
    Q_UNUSED( name )
    if( data.size() == 0 ) return;
    
    if( data.contains( "noscriptrunning" ) )
        m_lyrics->setPlainText( i18n( "No lyrics script is running!" ) );
    if( data.contains( "fetching" ) )
    {
        debug() << "fetching";
        m_lyrics->setPlainText( i18n( "Lyrics are being fetched." ) );
    }else if( data.contains( "error" ) )
        m_lyrics->setPlainText( i18n( "Lyrics were not able to be downloaded. Please check your internet connection." ) );
    else if( data.contains( "suggested" ) )
        m_lyrics->setPlainText( i18n( "Todo.... show suggestions here!" ) );
    else if( data.contains( "lyrics" ) )
    {
        QVariantList lyrics  = data[ "lyrics" ].toList();
        m_title->setText( lyrics[ 0 ].toString() );
        m_artist->setText( lyrics[ 1 ].toString() );
        m_site->setText( lyrics[ 2 ].toString() );
        
        m_lyrics->setPlainText( lyrics[ 3 ].toString() );
//         calculateHeight();
    }
    update();
}

void LyricsApplet::paintInterface( QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect )
{
    Q_UNUSED( option );
    
    QRect tmp( 0.0, 0.0, m_header->size().width(), 0.0 );
    tmp.setHeight( m_header->size().width() * m_headerAspectRatio );
/*    debug() << "finding height:" << m_header->size().width() << "*" << m_headerAspectRatio;
    debug() << "painting into rect:" << tmp << "contentsRect:" << contentsRect;*/
    m_header->paint( p, tmp, "lyricsheader" );

// align items
//     debug() << "size:" << m_header->size();
    m_lyricsLabel->setPos( m_header->elementRect( "title" ).topLeft());
    m_artistLabel->setPos( m_header->elementRect( "titlelabel" ).topLeft() );
    m_titleLabel->setPos( m_header->elementRect( "artistlabel" ).topLeft() );
    m_siteLabel->setPos( m_header->elementRect( "sitelabel" ).topLeft() );
    
    m_lyrics->setPos( m_header->elementRect( "lyrics" ).topLeft() );
    m_title->setPos( m_header->elementRect( "lyricstrackname" ).topLeft() );
    m_artist->setPos( m_header->elementRect( "lyricsartist" ).topLeft() );
    m_site->setPos( m_header->elementRect( "lyricslyricssite" ).topLeft() );
}

void LyricsApplet::calculateHeight()
{
//     debug() << "oldheight" << m_size.height();
    qreal lyricsheight = m_lyrics->boundingRect().height(); 
//     debug() << "checking if lyrics are too long for box:"
//         << lyricsheight
//         << m_theme->elementRect( "lyrics" );
    
    if( lyricsheight > m_header->elementRect( "lyrics" ).height() ) // too short
    {
        qreal expandBy = lyricsheight - m_header->elementRect( "lyrics" ).height();
//         debug() << "expanding by:" << expandBy;
        m_size.setHeight( m_size.height() + expandBy );
    } /*else if( lyricsheight < m_theme->elementRect( "lyrics" ).height() )
    { // too long
        qreal shrinkBy = m_theme->elementRect( "lyrics" ).height() - lyricsheight;
        debug() << "shrinking by:" << shrinkBy
            << "final height:" << m_size.height() - shrinkBy;
        m_size.setHeight( m_size.height() - shrinkBy );
    }*/
    
    m_header->resize( m_size );    
//     emit changed();
//     debug() << "newheight:" << m_size.height();
}

void LyricsApplet::resize( qreal newWidth )
{
    m_size.setWidth( newWidth );
    m_size.setHeight( m_header->size().height() );
//     m_size.setHeight( height );
    
//     calculateHeight();
    
    debug() << "set size to:" << m_size;
    m_header->resize( m_size );
    m_lyrics->setTextWidth( m_header->elementRect( "lyrics" ).width() );
    constraintsUpdated();
}

#include "LyricsApplet.moc"
