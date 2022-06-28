/*
To be able to define callbacks of different data types without
having to define a million different individual callbacks, a
more abstract structure is needed.

For this, the GUICallbackDatum struct is introduced. The callback
code can then cast the void pointer pData back to its original
type which can be inferred from the Type attribute in form of the enum
GUIInputType (and also, the callback code should already
know which data types it expects).

The GUICallbackObject is what will actually be passed to the
callback function. It includes the FormID of the set of options
it is registered to in order to allow for the same object to
potentially receive multiple callbacks. FormIDs are set during
initialization/creation of the GUI. Likewise, the integer indices
of the Data map are the OptionIDs registered during set up of
the respective form.

The callbacks themselves are handled through CForge's ITListener
and ITCaller interface templates, with the GUICallbackObject data
type.
 */

#pragma once

#include <unordered_map>

enum GUIInputType {
    INPUTTYPE_INT,          //returns integers
    INPUTTYPE_BOOL,         //bool
    INPUTTYPE_STRING,       //u32string
    INPUTTYPE_RANGESLIDER,  //float
    INPUTTYPE_DROPDOWN      //int
};

struct GUICallbackDatum {
    GUIInputType Type;
    void * pData;
};

struct GUICallbackObject {
    int FormID;
    std::unordered_map<int, GUICallbackDatum> Data;
};
