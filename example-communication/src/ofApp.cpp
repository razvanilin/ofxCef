#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    string url = "file://" + ofToDataPath("index.html", true);
    cef.setup(url);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Don't call the bind method before the render process is initialized
    // and V8 context is created – Should be an event
    if (!rendererInitialized && cef.isRendererInitialized()) {
        
        // Bind js function 'dataToOf' to C++ method 'ofApp::gotMessageFromJS'
        cef.bind("dataToOF", this, &ofApp::gotMessageFromJS);
        rendererInitialized = true;
    }
    
    // Globally update CEF
    updateCEF();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    cef.draw();
}

void ofApp::gotMessageFromJS(const ofxCEFJSMessageArgs& msg){
    
    ofLogNotice() << "gotMessageFromJS()";
    
    for (int i = 0; i < msg.args->GetSize(); i++) {
        CefValueType type = msg.args->GetType(i);
        ofLogNotice() << "  Message index " + ofToString(i) + " of type " + ofToString(type);
        
        switch (type) {
            case VTYPE_BOOL:
                ofLogNotice() << "  Bool content: " << ofToString(msg.args->GetBool(i));
                break;
            case VTYPE_INT:
                ofLogNotice() << "  Int content: " << ofToString(msg.args->GetInt(i));
                break;
            case VTYPE_DOUBLE:
                ofLogNotice() << "  Double content: " << ofToString(msg.args->GetDouble(i));
                break;
            case VTYPE_STRING:
                ofLogNotice() << "  String content: " << msg.args->GetString(i).ToString();
                break;
                
            default:
                ofLogNotice() << "  Might be a VTYPE_BINARY, VTYPE_DICTIONARY or VTYPE_LIST";
                break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 'r') {
        cef.reload();
        rendererInitialized = false;
    }
    else if (key == 'b') {
        cef.bind("dataToOF", this, &ofApp::gotMessageFromJS);
    }
    else {
        // Send data to JS every time a key is pressed
        cef.executeJS("dataFromOF(\"keyReleased in OF: " + ofToString(key) + "\")");
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
