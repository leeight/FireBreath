/**********************************************************\

  Auto-generated FBTestPlugin.cpp

  This file contains the auto-generated main plugin object
  implementation for the ${PLUGIN.name} project

\**********************************************************/
#ifndef H_TEMPLATEPLUGIN
#define H_TEMPLATEPLUGIN

#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/AttachedEvent.h"

#include "PluginCore.h"
#include "PluginWindow.h"

class FBTestPlugin : public FB::PluginCore
{
public:
    static void StaticInitialize();
    static void StaticDeinitialize();

public:
    FBTestPlugin();
    virtual ~FBTestPlugin();

public:
    virtual FB::JSAPIPtr createJSAPI();
    virtual bool IsWindowless() { return false; }
    
    std::string getPluginPath() { return m_filesystemPath; }

    bool isWindowless();

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::AttachedEvent, onAttached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::DetachedEvent, onDetached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::RefreshEvent, draw, FB::PluginWindow)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow*);
    virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow*);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow*);
    virtual bool onAttached(FB::AttachedEvent *evt, FB::PluginWindow*);
    virtual bool onDetached(FB::DetachedEvent *evt, FB::PluginWindow*);
    virtual bool draw(FB::RefreshEvent *evt, FB::PluginWindow*);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/
};

#endif
