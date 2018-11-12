#pragma once

#include "include/cef_app.h"
#include "include/cef_client.h"

//--------------------------------------------------------------
class ofxCEFClientAppBrowser : public CefApp, public CefBrowserProcessHandler
{
public:
    ofxCEFClientAppBrowser() {};
    ~ofxCEFClientAppBrowser() {};
private:

    // CefApp methods.
    void OnBeforeCommandLineProcessing(const CefString& process_type,
                                       CefRefPtr<CefCommandLine> command_line) OVERRIDE;
    
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {
        return this;
    }

    IMPLEMENT_REFCOUNTING(ofxCEFClientAppBrowser);
    DISALLOW_COPY_AND_ASSIGN(ofxCEFClientAppBrowser);

};
