#include "ofxCEFBrowserClient.h"
#include "ofxCEF.h"

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
