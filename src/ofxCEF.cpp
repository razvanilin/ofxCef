#include "ofxCEF.h"
#include "ofMain.h"
#include "ofAppGLFWWindow.h"
#if defined(TARGET_OSX)
#include <Cocoa/Cocoa.h>
#endif
#include "ofxCEFClientApp.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
#if defined(TARGET_OSX)
@interface NSNotificationManager: NSObject
{
    ofxCEF *observer;
    notificationHandler callback;
}

- (void)onChangeScreen:(id)sender;

@end

@implementation NSNotificationManager

- (id)initWithObserver:(ofxCEF *)obs method:(notificationHandler)m;
{
    self = [super init];
    if (self) {
        observer = obs;
        callback = m;
        
        NSWindow * cocoaWindow =  (NSWindow *) ((ofAppGLFWWindow *) ofGetWindowPtr())->getCocoaWindow();
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onChangeScreen:) name:NSWindowDidChangeScreenNotification
                                                   object:cocoaWindow];
    }
    return self;
}

- (void)onChangeScreen:(id)sender
{
    (observer->*callback)();
}

@end
#endif

//--------------------------------------------------------------
//--------------------------------------------------------------
#if defined(TARGET_WIN32)
HINSTANCE hInst;   // current instance
#endif

void initofxCEF(int argc, char** argv){

#if defined(TARGET_OSX)
    CefMainArgs main_args(argc, argv);
#elif defined(TARGET_WIN32)
    CefMainArgs main_args(::GetModuleHandle(NULL));
    
    
    // These flags must match the Chromium values.
    const char kProcessType[] = "type";
    const char kRendererProcess[] = "renderer";
#if defined(OS_LINUX)
    const char kZygoteProcess[] = "zygote";
#endif
    
    
    // Parse command-line arguments.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());
    
    ofLogNotice() << "Args: " << command_line->GetCommandLineString().ToString();
    
    // Create a ClientApp of the correct type.
    
    CefRefPtr<CefApp> app;
    
    // The command-line flag won't be specified for the browser process.
    if (command_line->HasSwitch(kProcessType)) {
        const std::string& process_type = command_line->GetSwitchValue(kProcessType);
        ofLogNotice() << "Process type: " << process_type;
        if (process_type == kRendererProcess) {
            app = new ofxCEFClientApp();
        }
#if defined(OS_LINUX)
        else if (process_type == kZygoteProcess) {
            return ZygoteProcess;
        }
#endif
        else {
            //app = new ClientAppOther();
        }
    }
    else {
        //app = new ClientAppBrowser();
    }
    
    // Execute the secondary process, if any.
    int exit_code = CefExecuteProcess(main_args, app, NULL);
    if (exit_code >= 0)
        return;
    
#endif
    
    //CefRefPtr<ofxCEFClientApp> app(new ofxCEFClientApp);

	CefSettings settings;
	settings.background_color = 0xFFFF00FF;
	settings.single_process = false; 
	settings.windowless_rendering_enabled = true;
	settings.command_line_args_disabled = true;
#if defined(TARGET_OSX)
    settings.remote_debugging_port = 8088;
    // On Windows this leads to:
    // tcp_socket_win.cc bind() retunred an error: an attempt was made to access a socket in a way forbidden by its access permissions
#endif
	//settings.multi_threaded_message_loop = true;

	CefInitialize(main_args, settings, NULL, NULL);
    
}


//--------------------------------------------------------------
void updateCEF(){
    GLint swapbytes, lsbfirst, rowlength, skiprows, skippixels, alignment;
    /* Save current pixel store state. */
    
    glGetIntegerv(GL_UNPACK_SWAP_BYTES, &swapbytes);
    glGetIntegerv(GL_UNPACK_LSB_FIRST, &lsbfirst);
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rowlength);
    glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skiprows);
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skippixels);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
    
    CefDoMessageLoopWork();
    //CefRunMessageLoop();
    
    /* Restore current pixel store state. */
    glPixelStorei(GL_UNPACK_SWAP_BYTES, swapbytes);
    glPixelStorei(GL_UNPACK_LSB_FIRST, lsbfirst);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, rowlength);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, skiprows);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, skippixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
}



//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxCEF::setup(const string& url, int width, int height){
    
	CefWindowInfo windowInfo;
	renderHandler = new ofxCEFRenderHandler();

#if defined(TARGET_OSX) 
	NSWindow * cocoaWindow =  (NSWindow *) ((ofAppGLFWWindow *) ofGetWindowPtr())->getCocoaWindow();
    [cocoaWindow setReleasedWhenClosed:NO];
    
    NSView * view =  [cocoaWindow contentView];
    windowInfo.SetAsWindowless(view);

	if (renderHandler->bIsRetinaDisplay) {
        ofSetWindowPosition(0, 0);
        ofSetWindowShape(ofGetWidth(), ofGetHeight());
    }

#elif defined(TARGET_WIN32)
	HWND hWnd = ofGetWin32Window();
	windowInfo.SetAsWindowless(hWnd);
#endif


	CefBrowserSettings settings;
	settings.webgl = STATE_ENABLED;
	settings.windowless_frame_rate = 60;
    settings.background_color = 0x00FFFFFF;
    settings.web_security = STATE_DISABLED;

	client = new ofxCEFBrowserClient(this, renderHandler);
    browser = CefBrowserHost::CreateBrowserSync(windowInfo, client.get(), url, settings, NULL);
    
    int reshapeSizeWidth, reshapeSizeHeight;
    
    if (width <= 0 && height <= 0) {
        fixedSize = false;
        reshapeSizeWidth = ofGetWidth();
        reshapeSizeHeight = ofGetHeight();
        
#if defined(TARGET_OSX)
        if (renderHandler->bIsRetinaDisplay) {
            reshapeSizeWidth = ofGetWidth()*2;
            reshapeSizeHeight = ofGetHeight()*2;
        }
#endif
        enableResize();
    }
    else {
        fixedSize = true;
        width_ = width;
        height_ = height;
        reshapeSizeWidth = width_;
        reshapeSizeHeight = height_;
    }
    
    // Is this resposible for eventually calling OnPaint,
    // and because the render handler takes a while this has to stay in some queue
    // and sometimes this doesn't work (and a simple resize fixes it)
    reshape(reshapeSizeWidth, reshapeSizeHeight);
    
    browerCreation = ofGetSystemTime();
    
    enableEvents();
}

//--------------------------------------------------------------
ofxCEF::ofxCEF(){
}

//--------------------------------------------------------------
ofxCEF::~ofxCEF(){
    exit();
}

void ofxCEF::exit() {
	//TODO Check if we need to do some calls to OnBeforeClose 
	disableEvents();
	renderHandler->bIsShuttingDown = true;
    browser->GetHost()->CloseBrowser(true);
    
    // The following call to CefShutdown make the app crash on OS X. Still not working on Windows neither.
	//CefShutdown();
}


//--------------------------------------------------------------
void ofxCEF::enableEvents(){
    ofAddListener(ofEvents().mousePressed, this, &ofxCEF::mousePressed);
    ofAddListener(ofEvents().mouseMoved, this, &ofxCEF::mouseMoved);
    ofAddListener(ofEvents().mouseDragged, this, &ofxCEF::mouseDragged);
    ofAddListener(ofEvents().mouseReleased, this, &ofxCEF::mouseReleased);
    ofAddListener(ofEvents().mouseScrolled, this , &ofxCEF::mouseScrolled);
    
    ofAddListener(ofEvents().keyPressed, this, &ofxCEF::keyPressed);
    ofAddListener(ofEvents().keyReleased, this, &ofxCEF::keyReleased);
}

//--------------------------------------------------------------
void ofxCEF::disableEvents(){
    ofRemoveListener(ofEvents().mousePressed, this, &ofxCEF::mousePressed);
    ofRemoveListener(ofEvents().mouseMoved, this, &ofxCEF::mouseMoved);
    ofRemoveListener(ofEvents().mouseDragged, this, &ofxCEF::mouseDragged);
    ofRemoveListener(ofEvents().mouseReleased, this, &ofxCEF::mouseReleased);
    ofRemoveListener(ofEvents().mouseScrolled, this , &ofxCEF::mouseScrolled);
    
    ofRemoveListener(ofEvents().keyPressed, this, &ofxCEF::keyPressed);
    ofRemoveListener(ofEvents().keyReleased, this, &ofxCEF::keyReleased);
}

//--------------------------------------------------------------
void ofxCEF::enableResize(){
    ofAddListener(ofEvents().windowResized, this, &ofxCEF::windowResized);
}

//--------------------------------------------------------------
void ofxCEF::disableResize(){
    ofRemoveListener(ofEvents().windowResized, this, &ofxCEF::windowResized);
}

//--------------------------------------------------------------
void ofxCEF::load(const char* url){
    if (!renderHandler->initialized) {
        renderHandler->init();
    }

    browser->GetMainFrame()->LoadURL(url);
}

void ofxCEF::reload() {
    V8ContextCreated = false;
    browser->Reload();
}

//--------------------------------------------------------------
void ofxCEF::draw(void){
    
    if (!isRendererInitialized()) { return; }
    
//    cout << "ofxCEF::draw "<< endl;
//    CefDoMessageLoopWork();
    
    // Alpha blending style. Texture values have premultiplied alpha.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    // Enable alpha blending.
    glEnable(GL_BLEND);
    
    //cout << ofGetWidth() << " - " << ofGetHeight() << endl;
    if (!fixedSize) {
        width_ = ofGetWidth();
        height_ = ofGetHeight();
    }

    ofMesh temp;
    temp.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    temp.addVertex( ofPoint(0,0) );
    temp.addTexCoord( ofVec2f(0,0) );
    temp.addVertex( ofPoint(width_,0) );
    temp.addTexCoord( ofVec2f(1,0) );
    temp.addVertex( ofPoint(0,height_) );
    temp.addTexCoord( ofVec2f(0,1) );
    temp.addVertex( ofPoint(width_,height_) );
    temp.addTexCoord( ofVec2f(1,1) );
    ofPushMatrix();
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, renderHandler->texture_id_);
    temp.draw();
    ofPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    
    ofEnableAlphaBlending();
}

//--------------------------------------------------------------
void ofxCEF::onLoadStart(){
    ofxCEFEventArgs evt;
    evt.type = ofxCEFEventArgs::onLoadStart;
    evt.httpStatusCode = -1;
    
    ofNotifyEvent(eventFromCEF, evt, this);
}

//--------------------------------------------------------------
void ofxCEF::onLoadEnd(int httpStatusCode){
    ofxCEFEventArgs evt;
    evt.type = ofxCEFEventArgs::onLoadEnd;
    evt.httpStatusCode = httpStatusCode;
    
    ofNotifyEvent(eventFromCEF, evt, this);
}

//--------------------------------------------------------------
void ofxCEF::bindCallFromJS(CefRefPtr<CefListValue> args) {
    ofxCEFJSMessageArgs msg;
    msg.args = args;
    
    ofNotifyEvent(messageFromJS, msg, this);
}

//--------------------------------------------------------------
void ofxCEF::notificationHandler(){

#if defined(TARGET_OSX) 
	float displayScale = [[NSScreen mainScreen] backingScaleFactor];
    
    cout << " ======= ofxCEF::notificationHandler =========" << endl;
    cout << "OF window size: " << ofGetWidth() << " - " << ofGetHeight() << endl;
    cout << "Changed Screen / displayScale :: " << displayScale << " ::/ frame orig : " << [NSScreen mainScreen].frame.origin.x << " - " << [NSScreen mainScreen].frame.origin.y << " ::/ size " << [NSScreen mainScreen].frame.size.width << " - " << [NSScreen mainScreen].frame.size.height << endl;
    
    bool scaleChanged = false;
    
    if (displayScale > 1.0){
        if (!renderHandler->bIsRetinaDisplay) {
            scaleChanged = true;
        }
        renderHandler->bIsRetinaDisplay = true;
        
    } else {
        if (renderHandler->bIsRetinaDisplay) {
            scaleChanged = true;
        }
        
        renderHandler->bIsRetinaDisplay = false;
    }
    
//    if (scaleChanged) {
//        if (renderHandler->bIsRetinaDisplay) {
//            reshape(ofGetWidth()*2, ofGetHeight()*2);
//        } else{
//            reshape(ofGetWidth(), ofGetHeight());
//        }
//    }
#elif defined(TARGET_WIN32)
	
#endif

    reshape(ofGetWidth(), ofGetHeight());
    renderHandler->init();
    browser->Reload();
}

//--------------------------------------------------------------
void ofxCEF::reshape(int w, int h){
    ofLogVerbose() << "Reshape: " << w << " - " << h;
    renderHandler->reshape(w, h);
    browser->GetHost()->WasResized();
}

//--------------------------------------------------------------
void ofxCEF::mousePressed(ofMouseEventArgs &e){
    int x = e.x;
    int y = e.y;
    
    browser->GetHost()->SendFocusEvent(true);

    if (renderHandler->bIsRetinaDisplay){
        x/=2;
        y/=2;
    }

    CefMouseEvent event;
    event.x = x;
    event.y = y;
    event.modifiers = 0;
    event.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
    browser->GetHost()->SendMouseClickEvent(event, btnType, false, 1);
}

//--------------------------------------------------------------
void ofxCEF::mouseReleased(ofMouseEventArgs &e){
    int x = e.x;
    int y = e.y;
    
    if (renderHandler->bIsRetinaDisplay){
        x/=2;
        y/=2;
    }

    CefMouseEvent event;
    event.x = x;
    event.y = y;
    event.modifiers = 0;
    event.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
    browser->GetHost()->SendMouseClickEvent(event, btnType, true, 1);

}

//--------------------------------------------------------------
void ofxCEF::mouseMoved(ofMouseEventArgs &e){
    int x = e.x;
    int y = e.y;
    
    if (renderHandler->bIsRetinaDisplay){
        x/=2;
        y/=2;
    }
    
    mouseX = x;
    mouseY = y;
    
    CefMouseEvent event;
    event.x = x;
    event.y = y;
    
    browser->GetHost()->SendMouseMoveEvent(event, false);
    
}

//--------------------------------------------------------------
void ofxCEF::mouseDragged(ofMouseEventArgs &e){
    mouseMoved(e);
}

//--------------------------------------------------------------
void ofxCEF::mouseScrolled(ofMouseEventArgs & mouse) {
    CefMouseEvent mouse_event;
    mouse_event.x = mouse.x;
    mouse_event.y = mouse.y;
    mouse_event.modifiers = 0;
    // Scrolling speed is slower than normal, why?
    browser->GetHost()->SendMouseWheelEvent(mouse_event, mouse.scrollX, mouse.scrollY);
}

//--------------------------------------------------------------
void ofxCEF::keyPressed(ofKeyEventArgs &e){
    //cout << "KEY:: " << e.key << " - KEYCODE:: " <<  e.keycode << " - SCANCODE::" << e.scancode << endl;
    
    CefKeyEvent event;
    
    if (e.key == OF_KEY_LEFT || e.key == OF_KEY_UP
        || e.key == OF_KEY_RIGHT || e.key == OF_KEY_DOWN
        || e.key == OF_KEY_BACKSPACE || e.key == OF_KEY_DEL) {

		event.windows_key_code = e.key;
        event.native_key_code = e.scancode;
        event.type = KEYEVENT_KEYDOWN;
        
    } else {
		event.windows_key_code = e.key;
        event.native_key_code = e.scancode;
        event.character = (char)e.key;
        event.type = KEYEVENT_CHAR;
        
    }
    
    browser->GetHost()->SendKeyEvent(event);
}

//--------------------------------------------------------------
void ofxCEF::keyReleased(ofKeyEventArgs &e){
    CefKeyEvent event;
    
    if (e.key == OF_KEY_LEFT || e.key == OF_KEY_UP
        || e.key == OF_KEY_RIGHT || e.key == OF_KEY_DOWN
        || e.key == OF_KEY_BACKSPACE || e.key == OF_KEY_DEL) {
        
        // Hack - Need to do this otherwise we loose an event.
		event.windows_key_code = e.key;
        event.native_key_code = e.scancode;
        event.character = (char)e.key;
        event.type = KEYEVENT_CHAR;
        browser->GetHost()->SendKeyEvent(event);
        
    } else {
		event.windows_key_code = e.key;
        event.native_key_code = e.scancode;
        event.type = KEYEVENT_KEYUP;
        browser->GetHost()->SendKeyEvent(event);
    }
}

//--------------------------------------------------------------
void ofxCEF::windowResized(ofResizeEventArgs &e){
    reshape(e.width, e.height);
    renderHandler->init();
    //cefgui->browser->Reload();
}


//--------------------------------------------------------------
void ofxCEF::executeJS(const string& command){
    CefRefPtr<CefFrame> frame = browser->GetMainFrame();
    frame->ExecuteJavaScript(command, frame->GetURL(), 0);
}
