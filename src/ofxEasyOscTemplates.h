#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include <functional>
#include <type_traits>

//*--------------------------------------------------------------------------------------------------*//

/// Three different pointer types (variables, functions and member functions) are wrapped into separate templated classes
/// which all inherit from a single abstract base class, so they can be stored inside a STL container.
/// Correct dispatching of OSC messages is handled via template specialization of the getData() method.

/// ofxOscListener

class ofxOscListener {
    public:
        virtual ~ofxOscListener() {}
        // generic dispatch method, implemented differently for ofxOscVariable and ofxOscMemberFunction
        virtual void dispatch(const ofxOscMessage & msg) = 0;
        virtual bool compare(ofxOscListener* listener) = 0;
        virtual bool isLambda() {
            return false;
        }

    protected:
        template<typename T>
        void getData(const ofxOscMessage& msg, int index, T& dest){
            cout << "Bad argument type for variable/function argument!\n";
        }
        // overload for STL containers
        template <typename T, template<typename E, typename Allocator = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<T>& dest){
            int length = msg.getNumArgs();
            dest.resize(length);

            auto it = dest.begin();
            for (int i = 0; i < length; ++i, ++it){
                getData<T>(msg, i, *it);
            }
        }
};

// template specialisation for each common data type


template<>
inline void ofxOscListener::getData<bool>(const ofxOscMessage& msg, int index, bool& dest) {
    bool arg{};
    if (msg.getNumArgs() > index){
        switch (msg.getArgType(index)){
        case OFXOSC_TYPE_FLOAT:
            arg = msg.getArgAsFloat(index);
            break;
        case OFXOSC_TYPE_INT32:
            arg = msg.getArgAsInt32(index);
            break;
        }
    };
    dest = arg;
}

template<>
inline void ofxOscListener::getData<unsigned char>(const ofxOscMessage& msg, int index, unsigned char& dest) {
    int arg{};
    if (msg.getNumArgs() > index){
        switch (msg.getArgType(index)){
            case OFXOSC_TYPE_FLOAT:
                arg = msg.getArgAsFloat(index);
                break;
            case OFXOSC_TYPE_INT32:
                arg = msg.getArgAsInt32(index);
                break;
            }
    }

    dest = max(0, min(255, arg)); // clamping and implicit conversion
}

template<>
inline void ofxOscListener::getData<int>(const ofxOscMessage& msg, int index, int& dest) {
    int arg{};
    if (msg.getNumArgs() > index){
        switch (msg.getArgType(index)){
            case OFXOSC_TYPE_FLOAT:
                arg = msg.getArgAsFloat(index);
                break;
            case OFXOSC_TYPE_INT32:
                arg = msg.getArgAsInt32(index);
                break;
            }
    }
    dest = arg;
}

template<>
inline void ofxOscListener::getData<float>(const ofxOscMessage& msg, int index, float& dest) {
    float arg{};
    if (msg.getNumArgs() > index){
        switch (msg.getArgType(index)){
            case OFXOSC_TYPE_FLOAT:
                arg = msg.getArgAsFloat(index);
                break;
            case OFXOSC_TYPE_INT32:
                arg = static_cast<float>(msg.getArgAsInt32(index));
                break;
            }
    }
    dest = arg;
}

template<>
inline void ofxOscListener::getData<double>(const ofxOscMessage& msg, int index, double& dest) {
    double arg{};
    if (msg.getNumArgs() > index){
        switch (msg.getArgType(index)){
            case OFXOSC_TYPE_FLOAT:
                arg = msg.getArgAsFloat(index);
                break;
            case OFXOSC_TYPE_INT32:
                arg = msg.getArgAsInt32(index);
                break;
            }
    }
    dest = arg;
}

template<>
inline void ofxOscListener::getData<string>(const ofxOscMessage& msg, int index, string& dest) {
    string arg{};
    if (msg.getNumArgs() > index){
        switch (msg.getArgType(index)){
            case OFXOSC_TYPE_STRING:
                arg = msg.getArgAsString(index);
                break;
            case OFXOSC_TYPE_FLOAT:
                arg = ofToString(msg.getArgAsFloat(index));
                break;
            case OFXOSC_TYPE_INT32:
                arg = ofToString(msg.getArgAsInt32(index));
                break;
            }
    }
    dest = std::move(arg);
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
                getData<T>(msg, 0, *var);
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


// class template specialization for void
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


// class template specialization for void
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
            // decay: remove constness and references to get the bare type
            typename std::decay<TArg>::type arg;
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


// class template specialization for void
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

