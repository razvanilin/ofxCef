#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    width = 100;
    height = 100;
}

//--------------------------------------------------------------
void ofApp::update(){
    updateCEF();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPushMatrix();
    for (auto& cef : cefs) {
        cef->draw();
        ofTranslate(width, 0);
    }
    ofPopMatrix();
    
    int textOffsetY = height + 20;
    
    ofDrawBitmapString("Press `n` to create new instances", 10, textOffsetY);
    ofDrawBitmapString("Press `d` to remove instances", 10, textOffsetY+20);
    ofDrawBitmapString("An empty square tells you something is wrong", 10, textOffsetY+40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 'n') {
        cefs.emplace_back(new ofxCEF());
        string url = "file://" + ofToDataPath("html/simple.html", true);
        url = "https://duckduckgo.org";
        cefs.back()->setup(url, width, height);
    }
    if (key == 'w') {
        for (auto& cef : cefs) {
            cef->reshape(width, height);
        }
    }
    if (key == 'r') {
        for (auto& cef : cefs) {
            cef->reload();
        }
    }
    if (key == 'e') {
        for (auto& cef : cefs) {
            cef->enableEvents();
        }
    }
    if (key == 'd') {
        if (cefs.size() > 0) {
            cefs.pop_back();
        }
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
