/****************************************************************************************
 * Copyright (c) 2007-2008 Maximilian Kossick <maximilian.kossick@googlemail.com>       *
 * Copyright (c) 2008 Daniel Caleb Jones <danielcjones@gmail.com>                       *
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

#include "XmlQueryReader.h"

#include "core/support/Debug.h"
#include "core-impl/collections/support/CollectionManager.h"

#include <QString>

struct XmlQueryReader::Private
{
    ReturnValueEnum flag;
    Collections::QueryMaker *qm;
    QList<Filter> filters;
};

Collections::QueryMaker*
XmlQueryReader::getQueryMaker( const QString &xmlData, ReturnValueEnum flag )
{
    Collections::QueryMaker *qm = CollectionManager::instance()->queryMaker();
    XmlQueryReader reader( qm, flag );
    if( reader.read( xmlData ) )
        return qm;
    else
        return 0;
}

XmlQueryReader::XmlQueryReader( Collections::QueryMaker *qm, ReturnValueEnum flag )
    : QXmlStreamReader()
    , d( new Private )
{
    d->flag = flag;
    d->qm = qm;
}

XmlQueryReader::~XmlQueryReader()
{
    delete d;
}

const QList<XmlQueryReader::Filter>&
XmlQueryReader::getFilters() const
{
    return d->filters;
}

bool
XmlQueryReader::read( const QString &xmlData )
{
    addData( xmlData );
    int queryCount = 0;
    while( !atEnd() )
    {
        readNext();

        if( isStartElement() )
        {
            //we expect exactly one query definition in the xml data.
            //so fail if we find more than one
            if( name() == "query" )
            {
                if( attributes().value( "version" ) == "1.0" )
                {
                    queryCount++;
                    readQuery();
                }
            }
        }
    }

    return queryCount == 1 && !error();
}

void
XmlQueryReader::readQuery()
{
    while( !atEnd() )
    {
        readNext();

        if( isStartElement() )
        {
            if( name() == "filters" )
                readFilters();
            else if( name() == "order" )
            {
                QXmlStreamAttributes attr = attributes();
                QStringRef fieldStr =  attr.value( "field" );
                QStringRef valueStr =  attr.value( "value" );

                if( valueStr == "random" )
                    d->qm->orderByRandom();
                else
                {
                    qint64 field = fieldVal( fieldStr );
                    bool descending = valueStr == "descending";

                    if( field != 0 )
                        d->qm->orderBy( field, descending  );
                }
            }
            else if( name() == "includeCollection" )
            {
                QStringRef id =  attributes().value( "id" );
                if( !id.isEmpty() )
                {
                    d->qm->includeCollection( id.toString() );
                }
            }
            else if( name() == "excludeCollection" )
            {
                QStringRef id =  attributes().value( "id" );
                if( !id.isEmpty() )
                {
                    d->qm->excludeCollection( id.toString() );
                }
            }
            else if( name() == "returnResultAsDataPtrs" )
            {
                d->qm->setReturnResultAsDataPtrs( true );
            }
            else if( name() == "limit" )
            {
                QStringRef value = attributes().value( "value" );
                if( !value.isEmpty() )
                    d->qm->limitMaxResultSize( value.toString().toInt() );
            }
            else if( name() == "onlyCompilations" )
            {
                d->qm->setAlbumQueryMode( Collections::QueryMaker::OnlyCompilations );
            }
            else if( name() == "onlyNormalAlbums" )
            {
                d->qm->setAlbumQueryMode( Collections::QueryMaker::OnlyNormalAlbums );
            }
            else if( name() == "onlyTrackArtists" )
            {
                d->qm->setArtistQueryMode( Collections::QueryMaker::TrackArtists );
            }
            else if( name() == "onlyAlbumArtists" )
            {
                d->qm->setArtistQueryMode( Collections::QueryMaker::AlbumArtists );
            }
            else if( name() == "AlbumOrTrackArtists" )
            {
                d->qm->setArtistQueryMode( Collections::QueryMaker::AlbumOrTrackArtists );
            }
            else if( name() == "returnValues" )
                readReturnValues();
            //add more container elements here
            else
                ignoreElements();
        }
    }
}

void
XmlQueryReader::ignoreElements()
{
    //let QXmlStreamReader worry about the fell-formedness of the document
    int depth = 1;
    while( !atEnd() && depth > 0 )
    {
        readNext();
        if( isEndElement() )
            depth--;
        if( isStartElement() )
            depth++;
    }
}

void
XmlQueryReader::readReturnValues()
{
    if( d->flag & XmlQueryReader::IgnoreReturnValues )
    {
        ignoreElements();
        return;
    }
    else
    {
        bool customQueryStarted = false;
        while( !atEnd() )
        {
            readNext();
            if( name() == "tracks" )
            {
                d->qm->setQueryType( Collections::QueryMaker::Track );
            }
            else if( name() == "artists" )
            {
                d->qm->setQueryType( Collections::QueryMaker::Artist );
            }
            else if( name() == "albums" )
            {
                d->qm->setQueryType( Collections::QueryMaker::Album );
            }
            else if( name() == "albumartist" )
            {
                d->qm->setQueryType( Collections::QueryMaker::AlbumArtist );
            }
            else if( name() == "genres" )
            {
                d->qm->setQueryType( Collections::QueryMaker::Genre );
            }
            else if( name() == "composers" )
            {
                d->qm->setQueryType( Collections::QueryMaker::Composer );
            }
            else if( name() == "year" )
            {
                d->qm->setQueryType( Collections::QueryMaker::Year );
            }
            else
            {
                if( !customQueryStarted )
                {
                    d->qm->setQueryType( Collections::QueryMaker::Custom );
                }
                //TODO write a mapping function somewhere
                if( name() == "title" )
                {
                    d->qm->addReturnValue( Meta::valTitle );
                }
                else if( name() == "artist" )
                {
                    d->qm->addReturnValue( Meta::valArtist );
                }
            }
        }
    }
}

void
XmlQueryReader::readAndOr()
{
    readFilters();
    ignoreElements();
    d->qm->endAndOr();
}

XmlQueryReader::Filter
XmlQueryReader::readFilter(QXmlStreamReader *reader)
{
    Filter filter;

    QXmlStreamAttributes attr = reader->attributes();

    filter.exclude = (reader->name() != "include");
    filter.field = fieldVal( attr.value( "field" ) );
    filter.value = attr.value( "value" ).toString();

    QStringRef compareStr = attr.value( "compare" );
    if( compareStr.isEmpty() )
        filter.compare = -1;
    else
        filter.compare = compareVal( compareStr );

    return filter;
}

void
XmlQueryReader::readFilters()
{
    while( !atEnd() )
    {
        readNext();
        if( isEndElement() )
        {
            if( name() == "and" || name() == "or" )
            {
                d->qm->endAndOr();
                break;
            }
            else if( name() == "filters" )
            {
                break;
            }
            else
                continue;
        }

        if( name() == "include" || name() == "exclude" )
        {
            Filter filter = readFilter(this);

            if( filter.field == 0 )
                break;

            if( filter.compare != -1 )
            {
                qint64 numValue = filter.value.toInt();
                if( !filter.exclude )
                {
                    debug() << "XQR: number include filter:";
                    d->qm->addNumberFilter( filter.field, numValue,
                            (Collections::QueryMaker::NumberComparison)filter.compare );
                }
                else
                {
                    debug() << "XQR: number exclude filter: ";
                    d->qm->excludeNumberFilter( filter.field, numValue,
                            (Collections::QueryMaker::NumberComparison)filter.compare );
                }
            }
            else
            {
                if( !filter.exclude )
                {
                    debug() << "XQR: include filter";
                    d->qm->addFilter( filter.field, filter.value );
                }
                else
                {
                    debug() << "XQR: exclude filter";
                    d->qm->excludeFilter( filter.field, filter.value );
                }
            }

            d->filters.append( filter );
        }
        else if( name() == "and" )
        {
            d->qm->beginAnd();
            readFilters();
        }
        else if( name() == "or" )
        {
            d->qm->beginOr();
            readFilters();
        }
    }
}

qint64
XmlQueryReader::fieldVal( QStringRef field )
{
    if     ( field == "url"        ) return Meta::valUrl;
    else if( field == "title"      ) return Meta::valTitle;
    else if( field == "artist"     ) return Meta::valArtist;
    else if( field == "album"      ) return Meta::valAlbum;
    else if( field == "albumartist") return Meta::valAlbumArtist;
    else if( field == "genre"      ) return Meta::valGenre;
    else if( field == "composer"   ) return Meta::valComposer;
    else if( field == "year"       ) return Meta::valYear;
    else if( field == "comment"    ) return Meta::valComment; 
    else if( field == "tracknr"    ) return Meta::valTrackNr;
    else if( field == "discnr"     ) return Meta::valDiscNr;
    else if( field == "length"     ) return Meta::valLength;
    else if( field == "bitrate"    ) return Meta::valBitrate;
    else if( field == "samplerate" ) return Meta::valSamplerate;
    else if( field == "filesize"   ) return Meta::valFilesize;
    else if( field == "format"     ) return Meta::valFormat;
    else if( field == "createdate" ) return Meta::valCreateDate;
    else if( field == "score"      ) return Meta::valScore;
    else if( field == "rating"     ) return Meta::valRating;
    else if( field == "firstplay"  ) return Meta::valFirstPlayed;
    else if( field == "lastplay"   ) return Meta::valLastPlayed;
    else if( field == "playcount"  ) return Meta::valPlaycount;
    else if( field == "label"      ) return Meta::valLabel;
    else                             return 0;
}

int
XmlQueryReader::compareVal( QStringRef compare )
{
    if( compare == "less" )
        return Collections::QueryMaker::LessThan;
    else if( compare == "greater" )
        return Collections::QueryMaker::GreaterThan;
    else if( compare == "equals" )
        return Collections::QueryMaker::Equals;
    else
        return -1;
}

