
/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2005, 2006, 2007 Eric Ehlers

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

#include <oh/exception.hpp>
#include <oh/utilities.hpp>
#include <ohxl/repositoryxl.hpp>
#include <ohxl/objectxl.hpp>
#include <ohxl/functioncall.hpp>
#include <ohxl/rangereference.hpp>
#include <ohxl/Conversions/opertoscalar.hpp>
#include <boost/algorithm/string.hpp>
/* Use BOOST_MSVC instead of _MSC_VER since some other vendors (Metrowerks,
   for example) also #define _MSC_VER
*/
#ifdef BOOST_MSVC
#  define BOOST_LIB_DIAGNOSTIC
#  include <xlsdk/auto_link.hpp>
#  undef BOOST_LIB_DIAGNOSTIC
#endif
#include <iomanip>
#include <sstream>
#include <string>

namespace ObjectHandler {

    extern Repository::ObjectMap objectMap_;

    typedef std::map<std::string, boost::shared_ptr<RangeReference> > ErrorMessageMap;
    ErrorMessageMap errorMessageMap_;

    typedef std::map<std::string, boost::shared_ptr<CallingRange> > RangeMap;
    RangeMap callingRanges_;

    RepositoryXL &RepositoryXL::instance() {
        if (instance_) {
            RepositoryXL *ret = dynamic_cast<RepositoryXL*>(instance_);
            if (ret) return *ret;
        }
        OH_FAIL("Attempt to reference uninitialized RepositoryXL object");
    }

    std::string RepositoryXL::storeObject(
            const std::string &objectID,
            const boost::shared_ptr<Object> &object) {
        boost::shared_ptr<ObjectHandler::ObjectXL> objectXL(
            new ObjectHandler::ObjectXL(objectID, object));

        if (FunctionCall::instance().callerType() == CallerType::Cell) {
            boost::shared_ptr<CallingRange> callingRange = getCallingRange();
            objectXL->setCallingRange(callingRange);
            callingRange->registerObject(objectXL);
        }

        ObjectMap::const_iterator result = objectMap_.find(objectXL->id());
        if (result != objectMap_.end()) {
            boost::shared_ptr<Object> oldObject = result->second;
            boost::shared_ptr<ObjectXL> oldObjectXL =
                boost::dynamic_pointer_cast<ObjectXL>(oldObject);
            OH_REQUIRE(objectXL->callerKey() == oldObjectXL->callerKey(),
                "Cannot create object with ID '" << objectXL->id() << "' in cell " <<
                objectXL->callerAddress() <<
                " because an object with that ID already resides in cell " <<
                oldObjectXL->callerAddress());
        }

        Repository::storeObject(objectXL->id(), objectXL);
        return objectXL->idFull();
    }

    boost::shared_ptr<Object> RepositoryXL::retrieveObjectImpl(
        const std::string &objectID) const {

            std::string idStrip = ObjectXL::getStub(objectID);
            boost::shared_ptr<Object> object = Repository::retrieveObjectImpl(idStrip);
            boost::shared_ptr<ObjectXL> objectXL = boost::dynamic_pointer_cast<ObjectXL>(object);
            return objectXL->object();

    }

    void RepositoryXL::deleteObject(const std::string &objectID) {
        Repository::deleteObject(ObjectXL::getStub(objectID));
    }

    void RepositoryXL::logError(const std::string& message, const bool &append) {
        
        std::ostringstream msgLog;
        if (FunctionCall::instance().callerType() == CallerType::Cell) {
            FunctionCall::instance().setError();
            std::ostringstream msgCell;
            msgCell << FunctionCall::instance().functionName() << " - " << message;

            std::string refStr = FunctionCall::instance().refStr();
            std::string refStrUpper = boost::algorithm::to_upper_copy(refStr);
            ErrorMessageMap::const_iterator i = errorMessageMap_.find(refStrUpper);
            if (i == errorMessageMap_.end()) {
                boost::shared_ptr<RangeReference> rangeReference(new RangeReference(refStrUpper));
                rangeReference->setErrorMessage(msgCell.str(), false);
                errorMessageMap_[refStrUpper] = rangeReference;
            } else {
                i->second->setErrorMessage(msgCell.str(), append);
            }

            msgLog << FunctionCall::instance().addressString() << " - ";
        }
        msgLog << FunctionCall::instance().functionName() << " - " << message;
        logMessage(msgLog.str(), 2);
    }

    std::string RepositoryXL::retrieveError(const XLOPER *xRangeRef) {

        OH_REQUIRE(xRangeRef->xltype == xltypeRef || xRangeRef->xltype == xltypeSRef,
            "Input parameter is not a range reference.");
        Xloper xRangeText;
        Excel(xlfReftext, &xRangeText, 1, xRangeRef);
        std::string refStr;
        operToScalar(xRangeText(), refStr);
        std::string refStrUpper = boost::algorithm::to_upper_copy(refStr);

        ErrorMessageMap::const_iterator i = errorMessageMap_.find(refStrUpper);
        if (i != errorMessageMap_.end())
            return i->second->errorMessage();

        RangeReference selectionReference(refStrUpper);
        for (i = errorMessageMap_.begin(); i != errorMessageMap_.end(); ++i) {
            if (i->second->contains(selectionReference))
                return i->second->errorMessage();
        }

        return "";

    }

    void RepositoryXL::clearError() {
        std::string refStr = FunctionCall::instance().refStr();
        errorMessageMap_.erase(boost::algorithm::to_upper_copy(refStr));
    }

    void RepositoryXL::dumpObject(const std::string &objectID, std::ostream &out) {
        ObjectMap::const_iterator result = objectMap_.find(ObjectXL::getStub(objectID));
        if (result == objectMap_.end()) {
            out << "no object in repository with ID = " << objectID << std::endl;
        } else {
            out << "log dump of object with ID = " << objectID << std::endl << result->second;
        }
    }

    void RepositoryXL::collectGarbage(const bool &deletePermanent) {

        RangeMap::iterator i = callingRanges_.begin();
        while (i != callingRanges_.end()) {
            boost::shared_ptr<CallingRange> callingRange = i->second;
            if (callingRange->valid()) {
                ++i;
            } else {
                callingRange->clearResidentObjects(deletePermanent);
                if (callingRange->empty())
                    callingRanges_.erase(i++);
                else
                    ++i;
            }
        }

    }

    boost::shared_ptr<CallingRange> RepositoryXL::getCallingRange() {

        Xloper xOldName;

        // get name if any

        Excel(xlfGetDef, &xOldName, 1, FunctionCall::instance().callerAddress());
        
        if (xOldName->xltype == xltypeStr) {   
            // if name - return associated CallingRange object
            std::string oldKey;
            operToScalar(xOldName(), oldKey);
            RangeMap::const_iterator i = callingRanges_.find(oldKey);
            OH_REQUIRE(i != callingRanges_.end(), "No calling range named " << oldKey);
            return i->second;
        } else {
            // no name - create new CallingRange object
             boost::shared_ptr<CallingRange> callingRange(new CallingRange);
            callingRanges_[callingRange->key()] = callingRange;
            return callingRange;
        }

    }

    void RepositoryXL::dump(std::ostream& out) {
        Repository::dump(out);

        out << std::endl << "calling ranges:";
        if (callingRanges_.empty()) {
            out << " none." << std::endl;
        } else {
            out << std::endl << std::endl;
            for (RangeMap::const_iterator i = callingRanges_.begin();
                    i != callingRanges_.end(); ++i) {
                out << i->second;
            }
        }
    }

}