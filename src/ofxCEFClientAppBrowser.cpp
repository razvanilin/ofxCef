#include "ofxCEFClientAppBrowser.h"


//--------------------------------------------------------------
void ofxCEFClientAppBrowser::OnBeforeCommandLineProcessing(const CefString& process_type,
                                                           CefRefPtr<CefCommandLine> command_line) {
    
    // Pass additional command-line flags to the browser process.
    if (process_type.empty()) {
        
        // Allow videos to autoplay
        command_line->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");
        
    }
    
    printf("Args (OnBeforeCommandLineProcessing Browser): %s", command_line->GetCommandLineString().ToString().c_str());
    
}

