#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <typeinfo>
// template classes used by ofxEasyOscReceiver
#include "ofxEasyOscTemplates.h"

//*-------------------------------------------------------------------------------------------------------*//
/// ofxEasyOscSender

/// Convenient class for sending OSC messages to Pure Data patches.

/// You can send a single message via the send() method.
/// Method chaining is supported: mySender.send("foo", x).send("bar", y);

class ofxEasyOscSender {
public:
    ofxEasyOscSender() {}
    ofxEasyOscSender(const string& address, int portNumber) { setup(address, portNumber); }

    void setup(const string& address, int portNumber) { sender.setup(address, portNumber); }
    // string argument
    ofxEasyOscSender& send(const string& address, const string& arg);
    // bool argument
    ofxEasyOscSender& send(const string& address, bool arg);
    // byte argument
    ofxEasyOscSender& send(const string& address, unsigned char arg);
    // int argument
    ofxEasyOscSender& send(const string& address, int arg);
    // float argument
    ofxEasyOscSender& send(const string& address, float arg);
	// bool vector argument
    ofxEasyOscSender& send(const string& address, const vector<bool>& arg);
    // byte vector argument
    ofxEasyOscSender& send(const string& address, const vector<unsigned char>& arg);
    // int vector argument
    ofxEasyOscSender& send(const string& address, const vector<int>& arg);
    // float vector argument
    ofxEasyOscSender& send(const string& address, const vector<float>& arg);
    // string vector argument
    ofxEasyOscSender& send(const string& address, const vector<string>& arg);
private:
    ofxOscSender sender;
};

//*----------------------------------------------------------------------------------------------------*//

/// ofxEasyOscReceiver:

/// Convenient class for receiving OSC messages from other applications.

/// You can register OSC addresses together with pointers to variables or member functions. Internally they are stored in an unordered map.
/// When calling update(), incoming OSC messages are compared to the map and if an address is found, the data is either written to a variable or
/// passed to a member functions accordingly.
/// Every registered OSC message is also stored in a multi-set, so the user can see which messages (and how many of them) have actually arrived since the last call to update().
/// You can write if statements with gotMessage(...) or ==(...) to do callback stuff if you don't like to register functions.
/// It's also possible to get the whole set and search it with count().

class ofxEasyOscReceiver {
public:
    ofxEasyOscReceiver() {}
    ofxEasyOscReceiver(int portNumber) { setup(portNumber); }

    void setup(int portNumber) { receiver.setup(portNumber);}

    // update the receiver (look for waiting OSC messages, write the data into the variables and put the addresses into the multi-set)
    void update();

    // return how often you received a specific message since the last update
    int gotMessage(const string address);

    // same as above
    int operator==(const string address);

    // get a multiset containing all addresses of messages that have arrived
    const unordered_multiset<string>& getIncomingMessages();

    /// The following types are allowed for variables, as arguments for functions and member function arguments:
    /// bool, unsigned char, int, float, double, string, vector<bool>, vector<unsigned char>, vector<int>, vector<float>, vector<double>, vector<string>.
    ///
    /// Member functions are supposed to take one of these types as their *only* argument (with any qualifiers) and return either void or bool.
    /// They can belong to an object or to the app itself (pass the 'this' pointer).
    ///
    /// Examples:
    ///
    /// add("/foo", &bar) ... will write the data of OSC messages with address "/foo" into variable 'bar'.
	/// add("/foo", someFunction) ... will pass the data of OSC messages with address "/foo" to the function someFunction (could also be a *static* member function)
    /// add("/foo", this, &ofApp::myFunction) ... will pass the data of OSC messages with address "/foo" to the member function ofApp::myFunction.
    /// add("/play", &myVideoPlayer, &ofVideoPlayer::play) ... an OSC message with address "/play" will call the play() method on an instance of ofVideoPlayer.
    ///
    /// If a member function is overloaded for several types, you can specify the desired type via a template argument:
    /// add<string>("/foo", &myImage, &ofImage::loadImage) ... tells the compiler that you want to call the version of loadImage() which takes a string.
    ///
    /// You can register more than one variable/member function for each address - your listeners will be stored in a list and notified in the same order
    /// you registered them. Method chaining is supported: e.g. myReceiver.add("/foo", &x).add("/bar", &y);
    ///
    /// Finally you can also unregister listeners.

    /// Examples:
    ///
    /// remove("/foo", &bar) ... will unregister variable 'bar' from the address "/foo"
    /// remove("/foo") ... will unregister all listeners from address "/foo"
    /// removeAll() ... will unregister everything


    /* register OSC addresses*/
    // looks up the address and adds new listeners to its callback list. if the address doesn't exist yet, it is added automatically

    // register address only (useful in conjunction with methods like gotMessage())
    ofxEasyOscReceiver& add(const string& address);

    // register variable
    template<typename T>
    ofxEasyOscReceiver& add(const string& address, T* var);

    // register free function taking no arguments
    template<typename TReturn>
    ofxEasyOscReceiver& add(const string& address, TReturn(*func)());

    // register free function taking a single argument
    template<typename TArg, typename TReturn>
    ofxEasyOscReceiver& add(const string& address, TReturn(*func)(TArg));

    // register lambda function taking no arguments
    ofxEasyOscReceiver& add(const string& address, const function<void()> & func);

    // register lambda function taking a single argument
    template<typename TArg>
    ofxEasyOscReceiver& add(const string& address, const function<void(TArg)> & func);

    // register member function taking no arguments
    template<typename TReturn, typename TObject>
    ofxEasyOscReceiver& add(const string& address, TObject* obj, TReturn(TObject::*func)());

    // register member function taking a single argument
    template<typename TArg, typename TReturn, typename TObject>
    ofxEasyOscReceiver& add(const string& address, TObject* obj, TReturn(TObject::*func)(TArg));

    /* unregister OSC addresses*/

    // tries to unregister a variable
    template<typename T>
    ofxEasyOscReceiver& remove(const string& address, T* var);

    // tries to unregister a function taking no argument
    template<typename TReturn>
    ofxEasyOscReceiver& remove(const string& address, TReturn(*func)());

    // tries to unregister a function taking a single argument
    template<typename TArg, typename TReturn>
    ofxEasyOscReceiver& remove(const string& address, TReturn(*func)(TArg));

    // tries to unregister a member function taking no argument
    template<typename TReturn, typename TObject>
    ofxEasyOscReceiver& remove(const string& address, TObject* obj, TReturn(TObject::*func)());

    // tries to unregister a member function taking a single argument
    template<typename TArg, typename TReturn, typename TObject>
    ofxEasyOscReceiver& remove(const string& address, TObject* obj, TReturn(TObject::*func)(TArg));

    // unregister all lambda functions associated with a certain address
    ofxEasyOscReceiver& removeLambdas(const string& address);

    // unregister *single* address with *all* its listeners from the map
    ofxEasyOscReceiver& remove(const string& address);

    // unregister *all* addresses with *all* its listeners from the map
    ofxEasyOscReceiver& removeAll();

protected:
    void searchAndRemove(const string& address, ofxOscListener* testobj);
    void searchAndRemoveLambdas(const string& address);

    unordered_map<string, list<unique_ptr<ofxOscListener>>> addressMap;
    ofxOscReceiver receiver;
    unordered_multiset<string> incomingMessages;
};



/* definitions */


// update the receiver (look for waiting OSC messages, write the data into the variables and put the addresses into the multi-set)
inline void ofxEasyOscReceiver::update(){
    incomingMessages.clear();

    while (receiver.hasWaitingMessages()) {
        ofxOscMessage msg;
        receiver.getNextMessage(msg);
        const string address = msg.getAddress();
        auto it = addressMap.find(address);

        if (it != addressMap.end()) {
            // pass OSC message to the list of listener objects
            auto & listeners = it->second;
            for (auto it = listeners.begin(); it != listeners.end(); ++it){
                (*it)->dispatch(msg);
            }

            // add address to multi-set
            incomingMessages.insert(address);
        }
    }
}

// return how often you received a specific message since the last update
inline int ofxEasyOscReceiver::gotMessage(const string address){
    return incomingMessages.count(address);
}

// same as above
inline int ofxEasyOscReceiver::operator==(const string address){
    return incomingMessages.count(address);
}

// get a multiset containing all addresses of messages that have arrived
inline const unordered_multiset<string>& ofxEasyOscReceiver::getIncomingMessages(){
    return incomingMessages;
}

/* register OSC addresses*/

// register address only (useful in conjunction with methods like gotMessage())
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address) {
    addressMap[address];
    return *this;
}
// register variable
template<typename T>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address, T* var){
    addressMap[address].push_back(unique_ptr<ofxOscListener>(new ofxOscVariable<T>(var)));
    return *this;
}
// register free function taking no arguments
template<typename TReturn>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address, TReturn(*func)()){
    addressMap[address].push_back(unique_ptr<ofxOscListener>(new ofxOscFunction<TReturn, void>(func)));
    return *this;
}
// register free function taking a single argument
template<typename TArg, typename TReturn>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address, TReturn(*func)(TArg)){
    addressMap[address].push_back(unique_ptr<ofxOscListener>(new ofxOscFunction<TReturn, TArg>(func)));
    return *this;
}
// register lambda function taking no arguments
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address, const function<void()> & func){
    addressMap[address].push_back(unique_ptr<ofxOscListener>(new ofxOscLambdaFunction<void>(func)));
    return *this;
}
// register lambda function taking a single argument
template<typename TArg>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address, const function<void(TArg)> & func){
    addressMap[address].push_back(unique_ptr<ofxOscListener>(new ofxOscLambdaFunction<TArg>(func)));
    return *this;
}
// register member function taking no arguments
template<typename TReturn, typename TObject>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address, TObject* obj, TReturn(TObject::*func)()){
    addressMap[address].push_back(unique_ptr<ofxOscListener>(new ofxOscMemberFunction<TObject, TReturn, void>(obj, func)));
    return *this;
}
// register member function taking a single argument
template<typename TArg, typename TReturn, typename TObject>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address, TObject* obj, TReturn(TObject::*func)(TArg)){
    addressMap[address].push_back(unique_ptr<ofxOscListener>(new ofxOscMemberFunction<TObject, TReturn, TArg>(obj, func)));
    return *this;
}

/* unregister OSC addresses*/

// tries to unregister a variable
template<typename T>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::remove(const string& address, T* var){
    // create a dummy object to test against
    ofxOscVariable<T> test(var);
    searchAndRemove(address, &test);
    return *this;
}
// tries to unregister a function taking no argument
template<typename TReturn>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::remove(const string& address, TReturn(*func)()){
    // create a dummy object to test against
    ofxOscFunction<TReturn, void> test(func);
    searchAndRemove(address, &test);
    return *this;
}
// tries to unregister a function taking a single argument
template<typename TArg, typename TReturn>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::remove(const string& address, TReturn(*func)(TArg)){
    // create a dummy object to test against
    ofxOscFunction<TReturn, TArg> test(func);
    searchAndRemove(address, &test);
    return *this;
}
// tries to unregister a member function taking no argument
template<typename TReturn, typename TObject>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::remove(const string& address, TObject* obj, TReturn(TObject::*func)()){
    // create a dummy object to test against
    ofxOscMemberFunction<TObject, TReturn, void> test(obj, func);
    searchAndRemove(address, &test);
    return *this;
}
// tries to unregister a member function taking a single argument
template<typename TArg, typename TReturn, typename TObject>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::remove(const string& address, TObject* obj, TReturn(TObject::*func)(TArg)){
    // create a dummy object to test against
    ofxOscMemberFunction<TObject, TReturn, TArg> test(obj, func);
    searchAndRemove(address, &test);
    return *this;
}

// unregister all lambda functions associated with a certain address
inline ofxEasyOscReceiver& ofxEasyOscReceiver::removeLambdas(const string& address){
    searchAndRemoveLambdas(address);
    return *this;
}

// unregister *single* address with *all* its listeners from the map
inline ofxEasyOscReceiver& ofxEasyOscReceiver::remove(const string& address){
    addressMap.erase(address);
    return *this;
}

// unregister *all* addresses with *all* its listeners from the map
inline ofxEasyOscReceiver& ofxEasyOscReceiver::removeAll(){
    addressMap.clear();
}



inline void ofxEasyOscReceiver::searchAndRemove(const string& address, ofxOscListener* testobj){
    auto found = addressMap.find(address);
    if (found != addressMap.end()){
        auto & listeners = found->second;
        auto it = listeners.begin();
        while (it != listeners.end()){
            // compare each listener with the test object
            if ((*it)->compare(testobj)){
                it = listeners.erase(it);
            } else {
                ++it;
            }
        }
    }
}

inline void ofxEasyOscReceiver::searchAndRemoveLambdas(const string& address){
    auto found = addressMap.find(address);
    if (found != addressMap.end()){
        auto & listeners = found->second;
        auto it = listeners.begin();
        while (it != listeners.end()){
            // check if listener is lambda
            if ((*it)->isLambda()){
                it = listeners.erase(it);
            } else {
                ++it;
            }
        }
    }
}
