/**********************************************************\ 
Original Author: Matthias (nitrogenycs)

Created:    Feb 28, 2010
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#include "NpapiPlugin.h"
#include "NpapiStream.h"
#include "config.h"
#undef min

using namespace FB::Npapi;

NpapiStream::NpapiStream( const std::string& url, bool cache, bool seekable, size_t internalBufferSize, NpapiBrowserHost* Host ) : FB::BrowserStream( url, cache, seekable, internalBufferSize ), /*internalBuffer( internalBufferSize ), */stream(0), host(Host)
{
}

NpapiStream::~NpapiStream()
{
    close();
}

bool NpapiStream::readRanges( const std::vector<Range>& ranges )
{
    if ( !getStream() || !isSeekable() || !isOpen() ) return false;
    if ( !ranges.size() ) return true;
    
    std::vector<NPByteRange> vecranges( ranges.size() );
    for ( size_t i = 0; i < ranges.size(); i++ )
    {
        NPByteRange& range = vecranges[i];
        range.offset = ranges[0].start;
        range.length = ranges[0].end - ranges[0].start;
        range.next = ( ( i + 1 ) < ranges.size() ) ? &vecranges[i+1] : 0;
    }

    return getHost()->RequestRead( getStream(), &vecranges[0] ) == NPERR_NO_ERROR;
}

bool NpapiStream::write(const char* data, size_t dataLength, size_t& written)
{
    if ( !getStream() || !isOpen() ) return false;
    written = getHost()->Write( getStream(), dataLength, const_cast<char*>(data) );
    return written == dataLength;
}

bool NpapiStream::close()
{
    if ( !getStream() ) return false;
    if ( isSeekable() && isOpen() )
    {
        StreamCompletedEvent ev(this, true);
        SendEvent( &ev );
    }
    setOpen( false );
    bool result = getHost()->DestroyStream( getStream(), NPRES_DONE ) == NPERR_NO_ERROR;
    setStream( 0 );
    return result;
}

int32_t NpapiStream::signalDataArrived(void* buffer, int32_t len, int32_t offset)
{
    size_t effectiveLen = std::min( getInternalBufferSize(), static_cast<size_t>(len) );
    if ( effectiveLen ) 
    {
        //memcpy( &internalBuffer[0], buffer, effectiveLen );
        
        float progress = 0;
        if ( getLength() )
        {
            progress = float( offset + len ) / float( getLength() ) * 100.f;
        }
        if ( isOpen() ) 
        {
            StreamDataArrivedEvent ev(this, buffer, effectiveLen, offset, progress);
            SendEvent( &ev );
        }
    }
    return effectiveLen;
}

void NpapiStream::signalOpened()
{
    setOpen( true );
    StreamOpenedEvent ev(this);
    SendEvent( &ev );
}

void NpapiStream::signalFailedOpen()
{
    StreamFailedOpenEvent ev(this);
    SendEvent( &ev );
}

void NpapiStream::signalCompleted(bool success)
{
    if ( isSeekable() && success ) return;      // If seekable, then complete is immediately sent after the open call worked. 
                                                        // But we don't want to signal completenes right then.

    setCompleted( true );

    /*if ( !isOpen() && !success )
    {
        signalFailedOpen();
    }*/

    close();

    StreamCompletedEvent ev(this, success);
    SendEvent( &ev );
}

void NpapiStream::signalCacheFilename(const std::wstring& CacheFilename)
{
    setCacheFilename( CacheFilename );
}

void NpapiStream::setStream(NPStream* Stream)
{
    stream = Stream;
}

NPStream* NpapiStream::getStream() const
{
    return stream;
}

NpapiBrowserHost* NpapiStream::getHost() const
{
    return host;
}