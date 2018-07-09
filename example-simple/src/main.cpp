#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    
    int argc = 0;
    char** argv = NULL;
    int exit_code = initofxCEF(argc, argv);
    if (exit_code >= 0) {
        return exit_code;
    }
    
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
    
    CefShutdown();
}
