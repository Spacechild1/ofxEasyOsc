#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include <functional>
#include <type_traits>
#include <typeinfo>

//*--------------------------------------------------------------------------------------------------*//

/// Three different pointer types (variables, functions and member functions) are wrapped into separate templated classes
/// which all inherit from a single abstract base class, so they can be stored inside a STL container.
/// Correct dispatching of OSC messages is handled via template specialization of the getData() method.

/// ofxOscListener

class ofxOscListener {
    public:
        virtual ~ofxOscListener() {}
        // generic dispatch method, implemented differently for ofxOscVariable and ofxOscMemberFunction
        virtual void dispatch(const ofxOscMessage& msg) = 0;
        virtual bool compare(ofxOscListener* listener) = 0;
        virtual bool isLambda() {
            return false;
        }

    protected:
        // get single argument
        template<typename T>
        void getData(const ofxOscMessage& msg, int index, T& dest){
            // catches wrong data types.
            // see template specializations for 'allowed' types
            cout << "Bad argument type for variable/function argument " << typeid(dest).name() << "!\n";
        }

        // overload for STL containers of simple one-dimensional types
        template <typename T, template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<T>& dest){
            int length = msg.getNumArgs();
            dest.resize(length);

            auto it = dest.begin();
            for (int i = 0; i < length; ++i, ++it){
                getData(msg, i, *it);
            }
        }
        // get list of ofVec2f objects
        template <template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<ofVec2f>& dest){
            getVec(msg, dest, 2);
        }
        // get list of ofVec3f objects
        template <template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<ofVec3f>& dest){
            getVec(msg, dest, 3);
        }
        // get list of ofVec4f objects
        template <template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<ofVec4f>& dest){
            getVec(msg, dest, 4);
        }
        // get list of ofMatrix3x3 objects
        template <template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<ofMatrix3x3>& dest){
            getVec(msg, dest, 9);
        }
        // get list of ofMatrix4x4 objects
        template <template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<ofMatrix4x4>& dest){
            getVec(msg, dest, 12);
        }
        // helper function for lists of ofVec2f, ofVec3f, ofVec4f, ofMatrix3x3 and ofMatrix4x4
        template <typename TVec, template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getVec(const ofxOscMessage& msg, Container<TVec>& dest, const int size){
            // N arguments can fill N/size objects (size is 2, 3, 4, 9 or 12)
            // integer division makes sure that only complete objects are created.
            const int length = msg.getNumArgs()/size;
            dest.resize(length);

            auto it = dest.begin();
            for (int i = 0; i < length; ++i, ++it){
                getData(msg, i * size, *it);
            }
        }
};

// template specialisation for each common data type


template<>
inline void ofxOscListener::getData<bool>(const ofxOscMessage& msg, int index, bool& dest) {
    if (msg.getNumArgs()){
        switch (msg.getArgType(index)){
        case OFXOSC_TYPE_FLOAT:
            dest = msg.getArgAsFloat(index);
            break;
        case OFXOSC_TYPE_INT32:
            dest = msg.getArgAsInt32(index);
            break;
        default:
            dest = false;
            break;
        }
    }
}

template<>
inline void ofxOscListener::getData<unsigned char>(const ofxOscMessage& msg, int index, unsigned char& dest) {
    if (msg.getNumArgs()){
        switch (msg.getArgType(index)){
        case OFXOSC_TYPE_FLOAT:
            dest = msg.getArgAsFloat(index);
            break;
        case OFXOSC_TYPE_INT32:
            dest = msg.getArgAsInt32(index);
            break;
        default:
            dest = 0;
            break;
        }
    }
}

template<>
inline void ofxOscListener::getData<int>(const ofxOscMessage& msg, int index, int& dest) {
    if (msg.getNumArgs()){
        switch (msg.getArgType(index)){
        case OFXOSC_TYPE_FLOAT:
            dest = msg.getArgAsFloat(index);
            break;
        case OFXOSC_TYPE_INT32:
            dest = msg.getArgAsInt32(index);
            break;
        default:
            dest = 0;
            break;
        }
    }
}

template<>
inline void ofxOscListener::getData<float>(const ofxOscMessage& msg, int index, float& dest) {
    if (msg.getNumArgs()){
        switch (msg.getArgType(index)){
        case OFXOSC_TYPE_FLOAT:
            dest = msg.getArgAsFloat(index);
            break;
        case OFXOSC_TYPE_INT32:
            dest = static_cast<float>(msg.getArgAsInt32(index));
            break;
        default:
            dest = 0;
            break;
        }
    }
}

template<>
inline void ofxOscListener::getData<double>(const ofxOscMessage& msg, int index, double& dest) {
    if (msg.getNumArgs()){
        switch (msg.getArgType(index)){
        case OFXOSC_TYPE_FLOAT:
            dest = msg.getArgAsFloat(index);
            break;
        case OFXOSC_TYPE_INT32:
            dest = msg.getArgAsInt32(index);
            break;
        default:
            dest = 0;
            break;
        }
    }
}

template<>
inline void ofxOscListener::getData<string>(const ofxOscMessage& msg, int index, string& dest) {
    if (msg.getNumArgs()){
        switch (msg.getArgType(index)){
        case OFXOSC_TYPE_STRING:
            dest = msg.getArgAsString(index);
            break;
        case OFXOSC_TYPE_FLOAT:
            dest = ofToString(msg.getArgAsFloat(index));
            break;
        case OFXOSC_TYPE_INT32:
            dest = ofToString(msg.getArgAsInt32(index));
            break;
        default:
            break;
        }
    }
}

template<>
inline void ofxOscListener::getData<ofVec2f>(const ofxOscMessage& msg, int index, ofVec2f& dest) {    
    if (msg.getNumArgs() >= 2){
        getData(msg, index, dest.x);
        getData(msg, index+1, dest.y);
    }
}

template<>
inline void ofxOscListener::getData<ofVec3f>(const ofxOscMessage& msg, int index, ofVec3f& dest) {
    if (msg.getNumArgs() >= 3){
        getData(msg, index, dest.x);
        getData(msg, index+1, dest.y);
        getData(msg, index+2, dest.z);
    }
}

template<>
inline void ofxOscListener::getData<ofVec4f>(const ofxOscMessage& msg, int index, ofVec4f& dest) {
    if (msg.getNumArgs() >= 4){
        getData(msg, index, dest.x);
        getData(msg, index+1, dest.y);
        getData(msg, index+2, dest.z);
        getData(msg, index+3, dest.w);
    }
}

template<>
inline void ofxOscListener::getData<ofMatrix3x3>(const ofxOscMessage& msg, int index, ofMatrix3x3& dest) {
    if (msg.getNumArgs() >= 9){
        for (int i = 0; i < 9; ++i){
            getData(msg, index+i, dest[i]);
        }
    }
}

template<>
inline void ofxOscListener::getData<ofMatrix4x4>(const ofxOscMessage& msg, int index, ofMatrix4x4& dest) {
    if (msg.getNumArgs() >= 12){
        for (int i = 0; i < 12; ++i){
            getData(msg, index+i, dest.getPtr()[i]);
        }
    }
}

// simply pass the OSC message
template<>
inline void ofxOscListener::getData<ofxOscMessage>(const ofxOscMessage& msg, int index, ofxOscMessage& dest) {
    dest = msg;
}

//*--------------------------------------------------------------------------------------------------*//

///ofxOscVariable
// generic dispatcher for variables
template<typename T>
class ofxOscVariable : public ofxOscListener {
    public:
        //constructor
        ofxOscVariable(T* var_) : var(var_) {}
        ~ofxOscVariable() {}
        // assigns OSC data to the variable.
        void dispatch(const ofxOscMessage & msg){
            if (var) {
                getData(msg, 0, *var);
			}
        }
        bool compare(ofxOscListener * listener){
            if (auto * ptr = dynamic_cast<ofxOscVariable<T>*>(listener)){
                return (var == ptr->var);
            } else {
                return false;
            }
        }
    protected:
        T* var;
};


//*--------------------------------------------------------------------------------------------------*//

/// ofxOscFunction
// generic dispatcher for functions (or *static* member functions).
template<typename TReturn, typename TArg>
class ofxOscFunction : public ofxOscListener {
    public:
        // constructor
        ofxOscFunction(TReturn(*func_)(TArg)) : func(func_) {}
        ~ofxOscFunction() {}
        void dispatch(const ofxOscMessage & msg){
            // decay: remove constness and references to get the bare type
            typename std::decay<TArg>::type arg;
            getData(msg, 0, arg);
            func(arg);
        }
        bool compare(ofxOscListener * listener) {
            if (auto * ptr = dynamic_cast<ofxOscFunction<TReturn, TArg>*>(listener)){
                return (func == ptr->func);
            } else {
                return false;
            }
        }

    protected:
        TReturn(*func)(TArg);
};


// partial specialization for void
template<typename TReturn>
class ofxOscFunction<TReturn, void> : public ofxOscListener {
    public:
        // constructor
        ofxOscFunction(TReturn(*func_)()) : func(func_) {}
        ~ofxOscFunction() {}
        void dispatch(const ofxOscMessage & msg){
            func();
        }
        bool compare(ofxOscListener * listener) {
            if (auto * ptr = dynamic_cast<ofxOscFunction<TReturn, void>*>(listener)){
                return (func == ptr->func);
            } else {
                return false;
            }
        }

    protected:
        TReturn(*func)();
};


//*--------------------------------------------------------------------------------------------------*//

/// ofxOscLambdaFunction
// generic dispatcher for lambda functions.
template<typename TArg>
class ofxOscLambdaFunction : public ofxOscListener {
    public:
        // constructor
        ofxOscLambdaFunction(const function<void(TArg)> & func_) : func(func_) {}
        ~ofxOscLambdaFunction() {}
        void dispatch(const ofxOscMessage & msg){
            // decay: remove constness and references to get the bare type
            typename std::decay<TArg>::type arg;
            getData(msg, 0, arg);
            func(arg);
        }
        bool compare(ofxOscListener * listener) {
            return false;
        }
        bool isLambda(){
            return true;
        }

    protected:
        function<void(TArg)> func;
};


// partial specialization for void
template<>
class ofxOscLambdaFunction<void> : public ofxOscListener {
    public:
        // constructor
        ofxOscLambdaFunction(const function<void()> & func_) : func(func_) {}
        ~ofxOscLambdaFunction() {}
        void dispatch(const ofxOscMessage & msg){
            func();
        }
        bool compare(ofxOscListener * listener) {
            return false;
        }
        bool isLambda(){
            return true;
        }

    protected:
        function<void()> func;
};


//*--------------------------------------------------------------------------------------------------*//

/// ofxOscMemberFunction
// generic dispatcher for (non-static) member functions.
template<typename TObject, typename TReturn, typename TArg>
class ofxOscMemberFunction : public ofxOscListener {
    public:
        // constructor
        ofxOscMemberFunction(TObject* obj_, TReturn(TObject::*func_)(TArg)) : obj(obj_), func(func_) {}
        ~ofxOscMemberFunction() {}
        void dispatch(const ofxOscMessage & msg){
            // remove constness and references to get the bare type
            typename decay<TArg>::type arg;
            getData(msg, 0, arg);
            if (obj) {
                (obj->*func)(arg);
            }
        }
        bool compare(ofxOscListener * listener) {
            if (auto * ptr = dynamic_cast<ofxOscMemberFunction<TObject, TReturn, TArg>*>(listener)){
                return (obj == ptr->obj && func == ptr->func);
            } else {
                return false;
            }
        }

    protected:
        TObject* obj;
        TReturn(TObject::*func)(TArg);
};


// partial specialization for void
template<typename TObject, typename TReturn>
class ofxOscMemberFunction<TObject, TReturn, void> : public ofxOscListener {
    public:
        ofxOscMemberFunction(TObject* obj_, TReturn(TObject::*func_)()) : obj(obj_), func(func_) {}
        ~ofxOscMemberFunction () {}
        void dispatch(const ofxOscMessage & msg){
            if (obj){
                (obj->*func)();
            }
        }
        bool compare(ofxOscListener * listener) {
            if (auto * ptr = dynamic_cast<ofxOscMemberFunction<TObject, TReturn, void>*>(listener)){
                return (obj == ptr->obj && func == ptr->func);
            } else {
                return false;
            }
        }

    protected:
        TObject* obj;
        TReturn(TObject::*func)();
};

