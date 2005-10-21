
/*
 Copyright (C) 2005 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <oh/objhandler.hpp>
#include <objectfoo.hpp>
#include <xlsdk/xlsdk.hpp>
#include <ohxl/conversions.hpp>

using namespace std;
using namespace ObjHandler;

DLLEXPORT int xlAutoOpen() {

    std::string xlErrorMessage;
    static XLOPER xDll;
    Excel(xlGetName, xlErrorMessage, &xDll, 0);

    Excel(xlfRegister, xlErrorMessage, 0, 7, &xDll,
        TempStrNoSize("\x0D""addin1MakeFoo"),   // function code name
        TempStrNoSize("\x05""CCCN#"),           // parameter codes
        TempStrNoSize("\x0D""addin1MakeFoo"),   // function display name
        TempStrNoSize("\x0A""handle,s,i"),      // comma-delimited list of parameters
        TempStrNoSize("\x01""1"),               // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)
        TempStrNoSize("\x07""Example"));        // function category

    Excel(xlFree, xlErrorMessage, 0, 1, &xDll);
    return 1;
}

DLLEXPORT char* addin1MakeFoo(char *handleStub, char *s, long *i) {
    try {
        obj_ptr objectPointer(new ObjectFoo(s, *i));
        const std::string handle = storeObject(handleStub, objectPointer);
        static char ret[XL_MAX_STR_LEN];
        ObjHandler::stringToChar(ret, handle);
        return ret;
    } catch (const std::exception &e) {
        logMessage(std::string("Error: addin1MakeFoo: ") + e.what(), 2);
        return 0;
    }
}

