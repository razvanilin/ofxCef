#include "ofxCEFV8ExtensionHandler.h"

#include "cefListV8Converter.hpp"
#include <iostream>
#include <string>

//--------------------------------------------------------------
ofxCEFV8ExtensionHandler::ofxCEFV8ExtensionHandler(CefRefPtr<CefApp> app){
    this->app = app;
}

//--------------------------------------------------------------
bool ofxCEFV8ExtensionHandler::Execute(const CefString &name,
                                       CefRefPtr<CefV8Value> object,
                                       const CefV8ValueList &arguments,
                                       CefRefPtr<CefV8Value> &retval,
                                       CefString &exception){
    
    
    std::string functionName = name;
    printf("Execute name: %s \n", functionName.c_str());
    
    if (bindJSFunctionNames.find(functionName) != bindJSFunctionNames.end()) {
        
        // Create the message object with a prefix + name of the function.
        // There is probably a more efficient way to combine these
        const std::string jsBindPrefix = "js-bind-";
        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(CefString(jsBindPrefix + functionName));
        
        // Populate the argument values.
            CefString type = arguments[0]->GetStringValue();
            
            // Create the message object.
            //CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(type);
            
            // Retrieve the argument list object.
            CefRefPtr<CefListValue> args = message->GetArgumentList();
            
            // Message mane
            args->SetString(0, arguments[1]->GetStringValue());
            
            // Message value
            if (type == "string") {
                args->SetString(1, arguments[2]->GetStringValue());
            }
            else if (type == "double") {
                args->SetDouble(1, arguments[2]->GetDoubleValue());
            }
            else if (type == "int") {
                args->SetInt(1, arguments[2]->GetIntValue());
            }
            else if (type == "bool") {
                args->SetBool(1, arguments[2]->GetBoolValue());
            }
            else {
                std::cout << "ofxCEFV8ExtensionHandler received a message of unknown type." << std::endl;
                return false;
            }
            
            // Send message
            CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
            browser->SendProcessMessage(PID_BROWSER, message);
            return true;

    } else if (name == "sendMessageToOF") {
        if (arguments.size() == 2 && arguments[0]->IsString()) {
            CefString type;
            
            if (arguments[1]->IsString()) {
                type = "string";
            }
            else if (arguments[1]->IsDouble()) {
                type = "double";
            }
            else if (arguments[1]->IsInt()) {
                type = "int";
            }
            else if (arguments[1]->IsBool()) {
                type = "bool";
            }
            else {
                std::cout << "ofxCEFV8ExtensionHandler received a message of unknown type." << std::endl;
                return false;
            }
            
            // Create the message object.
            CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(type);
            
            // Retrieve the argument list object.
            CefRefPtr<CefListValue> args = message->GetArgumentList();
            
            // Message mane
            args->SetString(0, arguments[0]->GetStringValue());
            
            // Message value
            if (type == "string") {
                args->SetString(1, arguments[1]->GetStringValue());
            }
            else if (type == "double") {
                args->SetDouble(1, arguments[1]->GetDoubleValue());
            }
            else if (type == "int") {
                args->SetInt(1, arguments[1]->GetIntValue());
            }
            else if (type == "bool") {
                args->SetBool(1, arguments[1]->GetBoolValue());
            }
            
            // Send message
            CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
            browser->SendProcessMessage(PID_BROWSER, message);
            return true;
            
        }
    }
    else if (name == "sendMessageWithTypeToOF") {
        if (arguments.size() == 3 && arguments[0]->IsString() && arguments[1]->IsString()) {
            // Populate the argument values.
            CefString type = arguments[0]->GetStringValue();
            
            // Create the message object.
            CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(type);
            
            // Retrieve the argument list object.
            CefRefPtr<CefListValue> args = message->GetArgumentList();
            
            // Message mane
            args->SetString(0, arguments[1]->GetStringValue());
            
            // Message value
            if (type == "string") {
                args->SetString(1, arguments[2]->GetStringValue());
            }
            else if (type == "double") {
                args->SetDouble(1, arguments[2]->GetDoubleValue());
            }
            else if (type == "int") {
                args->SetInt(1, arguments[2]->GetIntValue());
            }
            else if (type == "bool") {
                args->SetBool(1, arguments[2]->GetBoolValue());
            }
            else {
                std::cout << "ofxCEFV8ExtensionHandler received a message of unknown type." << std::endl;
                return false;
            }
            
            // Send message
            CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
            browser->SendProcessMessage(PID_BROWSER, message);
            return true;

         }
    }

    return false;
}
