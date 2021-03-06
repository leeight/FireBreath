/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include <algorithm>
#include "PluginEvent.h"
#include "PluginEventSink.h"
#include "PluginEventSource.h"
#include "PluginEvents/AttachedEvent.h"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>
#include <boost/lambda/bind.hpp>

using namespace FB;

PluginEventSource::PluginEventSource()
{
}

PluginEventSource::~PluginEventSource()
{
}

void PluginEventSource::AttachObserver(FB::PluginEventSink *sink)
{
    AttachObserver(sink->shared_ptr());
}

void PluginEventSource::AttachObserver( PluginEventSinkPtr sink )
{
    using namespace boost::lambda;
    boost::recursive_mutex::scoped_lock _l(m_observerLock);
    m_observers.push_back(sink);
    AttachedEvent newEvent;
    sink->HandleEvent(&newEvent, this);
}

void PluginEventSource::DetachObserver(FB::PluginEventSink *sink)
{
    DetachObserver(sink->shared_ptr());
}

bool PluginEventSource::_deleteObserver( PluginEventSinkPtr sink, PluginEventSinkWeakPtr wptr )
{
    PluginEventSinkPtr ptr(wptr.lock());
    if (!ptr) {
        return true;
    } else if (sink == ptr) {
        DetachedEvent evt;
        sink->HandleEvent(&evt, this);
        return true;
    } else {
        return false;
    }
}

void PluginEventSource::DetachObserver( PluginEventSinkPtr sink )
{
    using namespace boost::lambda;
    boost::recursive_mutex::scoped_lock _l(m_observerLock);
    m_observers.remove_if(bind<bool>(&PluginEventSource::_deleteObserver, this, var(sink), _1));
}

bool PluginEventSource::SendEvent(PluginEvent* evt)
{
    boost::recursive_mutex::scoped_lock _l(m_observerLock);
    ObserverMap::iterator it = m_observers.begin();
    while (it != m_observers.end()) {
        PluginEventSinkPtr tmp = it->lock();
        if (!tmp) {
            it = m_observers.erase(it); // Clear out weak_ptrs that don't point to anything alive
        }
        else if (tmp && tmp->HandleEvent(evt, this)) {
            return true;    // Tell the caller that the event was handled
        } else {
            it++;
        }
    }
    return false;
}
