#include "ofxCEFBrowserClient.h"
#include "ofxCEF.h"
#include "include/wrapper/cef_helpers.h"

//--------------------------------------------------------------
ofxCEFBrowserClient::ofxCEFBrowserClient(ofxCEF* parent, ofxCEFRenderHandler* renderHandler){
    _parent = parent;
    handler = renderHandler;
}

//--------------------------------------------------------------
CefRefPtr<CefRenderHandler> ofxCEFBrowserClient::GetRenderHandler(){
    return handler;
}

//--------------------------------------------------------------
CefRefPtr<CefLoadHandler> ofxCEFBrowserClient::GetLoadHandler(){
    return this;
}

CefRefPtr<CefLifeSpanHandler> ofxCEFBrowserClient::GetLifeSpanHandler(){
    return this;
}

//--------------------------------------------------------------
void ofxCEFBrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame){
    _parent->onLoadStart();
}


//--------------------------------------------------------------
void ofxCEFBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode){
    _parent->onLoadEnd(httpStatusCode);
}

//--------------------------------------------------------------
bool ofxCEFBrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefProcessId source_process,
                                             CefRefPtr<CefProcessMessage> message){
    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = message->GetArgumentList();

    // Retrieve the argument values.
    string name = message->GetName().ToString();
    
    ofLogVerbose() << "Message: " << name << " ; " << args;
    
    
    string jsBindPrefix = "js-bind-";
    if (name.compare(0, jsBindPrefix.size(), jsBindPrefix) == 0) {
        _parent->bindCallFromJS(args);
        return true;
    }
    else if (name == "OnV8ContextCreated") {
        _parent->V8ContextCreated = true;
    }
    else if (name == "OnV8ContextReleased") {
        _parent->V8ContextCreated = false;
    }

    return true;
}


void ofxCEFBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    // Must be executed on the UI thread.
    CEF_REQUIRE_UI_THREAD();
    
    if (!m_Browser.get())   {
        // Keep a reference to the main browser.
        m_Browser = browser;
        m_BrowserId = browser->GetIdentifier();
    }
    
    // Keep track of how many browsers currently exist.
    m_BrowserCount++;
}

void ofxCEFBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    // Must be executed on the UI thread.
    CEF_REQUIRE_UI_THREAD();
    
    if (m_BrowserId == browser->GetIdentifier()) {
        // Free the browser pointer so that the browser can be destroyed.
        m_Browser = NULL;
    }
    
    if (--m_BrowserCount == 0) {

    }
}
