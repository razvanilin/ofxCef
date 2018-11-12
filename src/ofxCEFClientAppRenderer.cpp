#include "ofxCEFClientAppRenderer.h"


//--------------------------------------------------------------
void ofxCEFClientAppRenderer::OnWebKitInitialized(){    
    std::string app_code =
        "var app;"
        "if (!app)"
        "    app = {};"
        "(function() {"
        "    app.sendMessageWithTypeToOF = function(type, name, value) {"
        "        native function sendMessageWithTypeToOF();"
        "        return sendMessageWithTypeToOF(type, name, value);"
        "    };"
        "    app.sendMessageToOF = function(name, value) {"
        "        native function sendMessageToOF();"
        "        return sendMessageToOF(name, value);"
        "    };"
        "})();;";

    //CefRegisterExtension( "v8/app", app_code, v8handler );
}


void ofxCEFClientAppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefV8Context> context) {
    v8context = context;
    // Retrieve the context's window object.
    //CefRefPtr<CefV8Value> globalObject = context->GetGlobal();
    
    //printf("context pointer: '%n' \n", context.get());
    
    // Create an instance of my CefV8Handler object.
    //CefRefPtr<CefV8Handler> v8handler = new MyV8Handler();
    
    // Create the "myfunc" function.
    //CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", v8handler);
    
    // Add the "myfunc" function to the "window" object.
    //globalObject->SetValue("myfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);
    
    printf("OnContextCreated\n");
    
    // Tell browser that V8 Context was created
    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(CefString("OnV8ContextCreated"));
    // Send message
    browser->SendProcessMessage(PID_BROWSER, message);
    
}

void ofxCEFClientAppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefV8Context> context) {
    v8context = NULL;
    // Tell browser that V8 Context was released
    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(CefString("OnV8ContextReleased"));
    // Send message
    browser->SendProcessMessage(PID_BROWSER, message);
}

bool ofxCEFClientAppRenderer::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                                   CefProcessId source_process,
                                                   CefRefPtr<CefProcessMessage> message){
    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = message->GetArgumentList();
    
    // Retrieve message name
    std::string name = message->GetName().ToString();
    
    
    printf("Renderer received message: %s \n", name.c_str());
    
    if (name == "js-make-bind") {
        
        // Check if context pointer is null. if nil store jsfunction name to bind them when the context was created.
        //CefRefPtr<CefV8Context> v8context = CefV8Context::GetCurrentContext();
        
        printf("Context is valid: '%p' \n", v8context.get());
        if (v8context.get()) {
            printf("arg size: '%zu' \n", args->GetSize());
            std::string jsFunctionName = args->GetString(0).ToString();
            
            printf("Context is valid: '%d' \n", v8context->IsValid());
            
            printf("Context enter: '%d' \n", v8context->Enter());
            
            
            CefRefPtr<CefV8Value> globalObject = v8context->GetGlobal();
            
            // Create the "myfunc" function.
            CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(jsFunctionName, v8handler);
            
            // Add the "myfunc" function to the "window" object.
            globalObject->SetValue(jsFunctionName, func, V8_PROPERTY_ATTRIBUTE_NONE);
            
            v8context->Exit();
        
            v8handler->bindJSFunctionNames.insert(jsFunctionName);
            
            printf("Created js function '%s' \n", jsFunctionName.c_str());
            
            return true;
        }
    }
    
    
    return false;
}

