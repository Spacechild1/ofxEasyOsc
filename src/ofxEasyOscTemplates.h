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
        // overload for STL containers of ofVec2f objects
        template <template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<ofVec2f>& dest){
            // ofVec2f needs two values, so N arguments can fill N/2 ofVec2f objects.
            // integer division makes sure that only complete ofVec2f objects are created.
            int length = msg.getNumArgs()/2;
            dest.resize(length);

            auto it = dest.begin();
            for (int i = 0; i < length; ++i, ++it){
                getData(msg, 2*i, *it);
            }
        }
        // overload for STL containers of ofVec3f objects
        template <template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<ofVec3f>& dest){
            // ofVec3f needs two values, so N arguments can fill N/3 ofVec3f objects.
            // integer division makes sure that only complete ofVec3f objects are created.
            int length = msg.getNumArgs()/3;
            dest.resize(length);

            auto it = dest.begin();
            for (int i = 0; i < length; ++i, ++it){
                getData(msg, 3*i, *it);
            }
        }
        // overload for STL containers of ofVec4f objects
        template <template <typename E, typename Allocater = std::allocator<E>> class Container>
        void getData(const ofxOscMessage& msg, int index, Container<ofVec4f>& dest){
            // ofVec4f needs two values, so N arguments can fill N/4 ofVec4f objects.
            // integer division makes sure that only complete ofVec4f objects are created.
            int length = msg.getNumArgs()/4;
            dest.resize(length);

            auto it = dest.begin();
            for (int i = 0; i < length; ++i, ++it){
                getData(msg, 4*i, *it);
            }
        }
};

// template specialisation for each common data type


template<>
inline void ofxOscListener::getData<bool>(const ofxOscMessage& msg, int index, bool& dest) {
    bool arg{};
    if (msg.getNumArgs()){
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
    if (msg.getNumArgs()){
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
    if (msg.getNumArgs()){
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
    if (msg.getNumArgs()){
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
    if (msg.getNumArgs()){
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
    if (msg.getNumArgs()){
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

template<>
inline void ofxOscListener::getData<ofVec2f>(const ofxOscMessage& msg, int index, ofVec2f& dest) {
    ofVec2f vec;

    if (msg.getNumArgs() >= 2){
            switch (msg.getArgType(index)){
                case OFXOSC_TYPE_FLOAT:
                    vec.x = msg.getArgAsFloat(index);
                    vec.y = msg.getArgAsFloat(index+1);
                    break;
                case OFXOSC_TYPE_INT32:
                    vec.x = msg.getArgAsInt32(index);
                    vec.y = msg.getArgAsInt32(index+1);
                    break;
                }
    }

    dest = vec;
}

template<>
inline void ofxOscListener::getData<ofVec3f>(const ofxOscMessage& msg, int index, ofVec3f& dest) {
    ofVec3f vec;

    if (msg.getNumArgs() >= 3){
            switch (msg.getArgType(index)){
                case OFXOSC_TYPE_FLOAT:
                    vec.x = msg.getArgAsFloat(index);
                    vec.y = msg.getArgAsFloat(index+1);
                    vec.z = msg.getArgAsFloat(index+2);
                    break;
                case OFXOSC_TYPE_INT32:
                    vec.x = msg.getArgAsInt32(index);
                    vec.y = msg.getArgAsInt32(index+1);
                    vec.z = msg.getArgAsInt32(index+2);
                    break;
                }
    }

    dest = vec;
}

template<>
inline void ofxOscListener::getData<ofVec4f>(const ofxOscMessage& msg, int index, ofVec4f& dest) {
    ofVec4f vec;

    if (msg.getNumArgs() >= 4){
            switch (msg.getArgType(index)){
                case OFXOSC_TYPE_FLOAT:
                    vec.x = msg.getArgAsFloat(index);
                    vec.y = msg.getArgAsFloat(index+1);
                    vec.z = msg.getArgAsFloat(index+2);
                    vec.w = msg.getArgAsFloat(index+3);
                    break;
                case OFXOSC_TYPE_INT32:
                    vec.x = msg.getArgAsInt32(index);
                    vec.y = msg.getArgAsInt32(index+1);
                    vec.z = msg.getArgAsInt32(index+2);
                    vec.w = msg.getArgAsInt32(index+3);
                    break;
                }
    }

    dest = vec;
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

