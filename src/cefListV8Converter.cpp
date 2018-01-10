//
//  cefListV8Converter.cpp
//  cef-test
//
//  Created by Michael Baisch on 13.05.17.
//
//

#include "cefListV8Converter.hpp"


// Forward declarations.


// Transfer a V8 value to a List index.
void convertListValue(CefRefPtr<CefListValue> list, int index,
                  CefRefPtr<CefV8Value> value) {
    if (value->IsArray()) {
        CefRefPtr<CefListValue> new_list = CefListValue::Create();
        convertList(value, new_list);
        list->SetList(index, new_list);
    } else if (value->IsString()) {
        list->SetString(index, value->GetStringValue());
    } else if (value->IsBool()) {
        list->SetBool(index, value->GetBoolValue());
    } else if (value->IsInt()) {
        list->SetInt(index, value->GetIntValue());
    } else if (value->IsDouble()) {
        list->SetDouble(index, value->GetDoubleValue());
    }
}

// Transfer a V8 array to a List.
void convertList(CefRefPtr<CefV8Value> source, CefRefPtr<CefListValue> target) {
    if (!source->IsArray()) { return; }
    
    int sourceSize = source->GetArrayLength();
    if (sourceSize == 0) { return; }
    
    target->SetSize(sourceSize);
    
    for (int i = 0; i < sourceSize; ++i) {
        convertListValue(target, i, source->GetValue(i));
    }
}

void convertList(const CefV8ValueList& source, CefRefPtr<CefListValue> target) {
    
    int sourceSize = source.size();
    if (sourceSize == 0) { return; }
    
    target->SetSize(sourceSize);
    
    int index = 0;
    for (auto element: source) {
        convertListValue(target, index, element);
    }

}

// Transfer a List value to a V8 array index.
void SetListValue(CefRefPtr<CefV8Value> list, int index,
                  CefRefPtr<CefListValue> value) {
    CefRefPtr<CefV8Value> new_value;
    
    CefValueType type = value->GetType(index);
    switch (type) {
        case VTYPE_LIST: {
            CefRefPtr<CefListValue> list = value->GetList(index);
            new_value = CefV8Value::CreateArray(list->GetSize());
            SetList(list, new_value);
        } break;
        case VTYPE_BOOL:
            new_value = CefV8Value::CreateBool(value->GetBool(index));
            break;
        case VTYPE_DOUBLE:
            new_value = CefV8Value::CreateDouble(value->GetDouble(index));
            break;
        case VTYPE_INT:
            new_value = CefV8Value::CreateInt(value->GetInt(index));
            break;
        case VTYPE_STRING:
            new_value = CefV8Value::CreateString(value->GetString(index));
            break;
        default:
            break;
    }
    
    if (new_value.get()) {
        list->SetValue(index, new_value);
    } else {
        list->SetValue(index, CefV8Value::CreateNull());
    }
}

// Transfer a List to a V8 array.
void SetList(CefRefPtr<CefListValue> source, CefRefPtr<CefV8Value> target) {
    //ASSERT(target->IsArray());
    
    int arg_length = source->GetSize();
    if (arg_length == 0)
        return;
    
    for (int i = 0; i < arg_length; ++i)
        SetListValue(target, i, source);
}
