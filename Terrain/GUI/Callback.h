/*
To be able to define callbacks of different data types without
having to define a million different individual callbacks, a
more abstract structure is needed.

For this, the CallbackDatum struct is introduced. The callback
code can then cast the void pointer pData back to its original
type which is held in the Type attribute in form of the enum
CallbackDatatype (and also, the callback code should already
know which data types it expects).

The CallbackObject is what will actually be passed to the
callback function. It includes the FormID of the set of options
it is registered to in order to allow for the same object to
potentially receive multiple callbacks. FormIDs are set during
initialization/creation of the GUI. Likewise, the integer indices
of the Data map are the OptionIDs registered during set up of
the respective form.

The callbacks themselves are handled through CForge's ITListener
and ITCaller interface templates, with the CallbackObject data
type.
 */

#pragma once

#include <unordered_map>

enum CallbackDatatype {
    DATATYPE_INT,
    DATATYPE_BOOLEAN,
    DATATYPE_STRING
};

struct CallbackDatum {
    CallbackDatatype Type;
    void * pData;
};

struct CallbackObject {
    int FormID;
    std::unordered_map<int, CallbackDatum> Data;
};
