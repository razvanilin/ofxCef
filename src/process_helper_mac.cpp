// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/cef_app.h"
#include "include/wrapper/cef_library_loader.h"
#include "ofxCEFClientAppBrowser.h"
#include "ofxCEFClientAppRenderer.h"


// These flags must match the Chromium values.
const char kProcessType[] = "type";
const char kRendererProcess[] = "renderer";
#if defined(OS_LINUX)
const char kZygoteProcess[] = "zygote";
#endif

//--------------------------------------------------------------
// Entry point function for sub-processes.
int main(int argc, char* argv[]){
    
    for (int i = 0; i < argc; i++){
        printf("%s \n", argv[i]);
    }
    
    // Load the CEF framework library at runtime instead of linking directly
    // as required by the macOS sandbox implementation.
    CefScopedLibraryLoader library_loader;
    if (!library_loader.LoadInHelper()) {
        return 1;
    }
    
    // Provide CEF with command-line arguments.
    CefMainArgs main_args(argc, argv);
    
    
    // Parse command-line arguments.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromArgv(argc, argv);
    
    // Create a ClientApp of the correct type.
    CefRefPtr<CefApp> app;
    
    if (!command_line->HasSwitch(kProcessType)) {
        app = new ofxCEFClientAppBrowser();
    }
    else {
        const std::string& process_type = command_line->GetSwitchValue(kProcessType);
        if (process_type == kRendererProcess) {
            app = new ofxCEFClientAppRenderer();
        }
    #if defined(OS_LINUX)
        else if (process_type == kZygoteProcess) {}
    #endif
    }
    

    // Execute the sub-process.
    return CefExecuteProcess(main_args, app.get(), NULL);
}
