//
//  cefListV8Converter.hpp
//  cef-test
//
//  Created by Michael Baisch on 13.05.17.
//
//

#ifndef cefListV8Converter_hpp
#define cefListV8Converter_hpp

#include <stdio.h>
#include "include/cef_v8.h"


void convertListValue(CefRefPtr<CefListValue> list, int index,
                  CefRefPtr<CefV8Value> value);

void convertList(CefRefPtr<CefV8Value> source, CefRefPtr<CefListValue> target);

void convertList(const CefV8ValueList& source, CefRefPtr<CefListValue> target);

void SetListValue(CefRefPtr<CefV8Value> list, int index,
                  CefRefPtr<CefListValue> value);
void SetList(CefRefPtr<CefListValue> source, CefRefPtr<CefV8Value> target);


#endif /* cefListV8Converter_hpp */
