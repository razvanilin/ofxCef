#ifndef CEFGUI_H
#define CEFGUI_H

#include "cef_app.h"

#include "ofxCEFBrowserClient.h"
#include "ofxCEFRenderHandler.h"
#include "ofBaseTypes.h"

//--------------------------------------------------------------
class ofxCEFMessageArgs : public ofEventArgs{
public:
    string name;
    string type;
    string value;
};

class ofxCEFJSMessageArgs : public ofEventArgs{
public:
    CefRefPtr<CefListValue> args;
};

//--------------------------------------------------------------
class ofxCEFEventArgs : public ofEventArgs{
public:
    enum Type{
        onLoadStart,
        onLoadEnd
    } type;
    int httpStatusCode;
};

//--------------------------------------------------------------
class ofxCEF;

void initofxCEF(int argc, char** argv);
void updateCEF();

//--------------------------------------------------------------
class ofxCEF : public ofBaseDraws
{
public:
    ofxCEF();
    ~ofxCEF();
    
    ofxCEF(const ofxCEF& other) = delete;
    ofxCEF & operator=(const ofxCEF&) = delete;
    
	void exit();

    void load(const char*);
    void reload();
    
    using ofBaseDraws::draw;
    
    void draw() const {
        draw(0,0);
    }
    void draw(float x, float y, float w, float h) const;
    void reshape(int, int);
    
    void setup(const string& url = "", int width = 0, int height = 0);
    
    void enableEvents();
    void disableEvents();
    
    void enableResize();
    void disableResize();
    
    void executeJS(const string& command);

    void notificationHandler();
    
    void onLoadStart();
    void onLoadEnd(int httpStatusCode);
    
    template <typename ArgumentsType, class ListenerClass>
    void bind(const string& functionName, ListenerClass * listener, void (ListenerClass::*listenerMethod)(ArgumentsType&), int prio = OF_EVENT_ORDER_AFTER_APP);

    // Don't call this
    void bindCallFromJS(CefRefPtr<CefListValue> args);
    
    ofEvent<ofxCEFEventArgs> eventFromCEF;
    
    bool V8ContextCreated = false; // Don't set this
    bool isReady() const { return V8ContextCreated && renderHandler->initialized && browser(); }
 
    /// \brief Get the height.
    /// \returns the height.
    float getHeight() const { return height_; }
    
    /// \brief Get the width.
    /// \returns the width.
    float getWidth() const { return width_; }
    
 private:
    
    CefRefPtr<CefBrowser> browser() const { return client->GetBrowser(); }
    
    void mousePressed(ofMouseEventArgs &e);
    void mouseReleased(ofMouseEventArgs &e);
    void mouseMoved(ofMouseEventArgs &e);
    void mouseDragged(ofMouseEventArgs &e);
    void mouseScrolled(ofMouseEventArgs &e);
    
    void keyPressed(ofKeyEventArgs &e);
    void keyReleased(ofKeyEventArgs &e);
    
    void windowResized(ofResizeEventArgs &e);
    
    int mouseX, mouseY;

    CefRefPtr<ofxCEFBrowserClient> client;

    ofxCEFRenderHandler* renderHandler;
    
    bool fixedSize;
    float width_, height_;

    uint64_t browerCreation;
    
    ofEvent<const ofxCEFJSMessageArgs&> messageFromJS;
};

typedef void (ofxCEF::*notificationHandler)();

template <typename ArgumentsType, class ListenerClass>
void ofxCEF::bind(const string& functionName, ListenerClass * listener, void (ListenerClass::*listenerMethod)(ArgumentsType&), int prio) {
    
    // Tell renderer process that we want
    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("js-make-bind");
    
    // Retrieve the argument list object. Set the function name
    CefRefPtr<CefListValue> args = message->GetArgumentList();
    args->SetSize(1);
    args->SetString(0, functionName);
    
    // Send the message to the render process
    if (browser()) {
        browser()->SendProcessMessage(PID_RENDERER, message);
    }
    
    ofAddListener(messageFromJS, listener, listenerMethod, prio);
}

#endif
