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
    ofxEasyOscSender(const string& address, int portNumber) { sender.setup(address, portNumber); }

    void setup(const string& address, int portNumber) { sender.setup(address, portNumber); }
	
    template <typename... Args>
    ofxEasyOscSender& send(const string& address, const Args&... args);
    
protected:
    ofxOscSender sender;
	
	// string argument
    template <typename... Args>
    void fill(ofxOscMessage& msg, const string& arg, const Args&... remain);

    // bool argument
    template <typename... Args>
    void fill(ofxOscMessage& msg, bool arg, const Args&... remain);

    // byte argument
    template <typename... Args>
    void fill(ofxOscMessage& msg, unsigned char arg, const Args&... remain);

    // int argument
    template <typename... Args>
    void fill(ofxOscMessage& msg, int arg, const Args&... remain);

    // float argument
    template <typename... Args>
    void fill(ofxOscMessage& msg, float arg, const Args&... remain);

    // double argument
    template <typename... Args>
    void fill(ofxOscMessage& msg, double arg, const Args&... remain);

    // ofVec2f argument
    template <typename... Args>
    void fill(ofxOscMessage& msg, const ofVec2f& arg, const Args&... remain);

    // ofVec3f argument
    template <typename... Args>
    void fill(ofxOscMessage& msg, const ofVec3f& arg, const Args&... remain);

    // ofVec4f argument
    template <typename... Args>
    void fill(ofxOscMessage& msg, const ofVec4f& arg, const Args&... remain);

    // STL container argument
    template <typename T,
            template <typename E, typename Allocator = std::allocator<E>> class Container,
              typename... Args>
    void fill(ofxOscMessage& msg, const Container<T>& vec, const Args&... remain);

    // dummy
    void fill(ofxOscMessage& msg);
};

// send a OSC message
template <typename... Args>
inline ofxEasyOscSender& ofxEasyOscSender::send(const string& address, const Args&... args){
	ofxOscMessage msg;
    msg.setAddress(address);

    if (sizeof...(args)){
        fill(msg, args...);
    }

	sender.sendMessage(msg);
    return *this;
}

// add string arg:
template <typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, const string& arg, const Args&... remain){
    msg.addStringArg(arg);

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

// add bool arg:
template <typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, bool arg, const Args&... remain){
    msg.addIntArg(arg);

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

// add byte arg:
template <typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, unsigned char arg, const Args&... remain){
    msg.addIntArg(arg);

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

// add int arg:
template <typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, int arg, const Args&... remain){
    msg.addIntArg(arg);

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

// add float arg:
template <typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, float arg, const Args&... remain){
    msg.addFloatArg(arg);

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

// add double arg:
template <typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, double arg, const Args&... remain){
    msg.addFloatArg(arg);

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

// add ofVec2f arg:
template <typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, const ofVec2f& arg, const Args&... remain){
    msg.addFloatArg(arg.x);
    msg.addFloatArg(arg.y);

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

// add ofVec3f arg:
template <typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, const ofVec3f& arg, const Args&... remain){
    msg.addFloatArg(arg.x);
    msg.addFloatArg(arg.y);
    msg.addFloatArg(arg.z);

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

// add ofVec4f arg:
template <typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, const ofVec4f& arg, const Args&... remain){
    msg.addFloatArg(arg.x);
    msg.addFloatArg(arg.y);
    msg.addFloatArg(arg.z);
    msg.addFloatArg(arg.w);

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

// add STL container arg:
template <typename T,
        template <typename E, typename Allocator = std::allocator<E>> class Container,
          typename... Args>
inline void ofxEasyOscSender::fill(ofxOscMessage& msg, const Container<T>& vec, const Args&... remain){
    const int length = vec.size();

    for (int i = 0; i < length; ++i){
        fill(msg, vec[i]);
    }

    if (sizeof...(remain)){
        fill(msg, remain...);
    }
}

	
// dummy
inline void ofxEasyOscSender::fill(ofxOscMessage& msg){
    cout << "I'm a dummy!\n";
}


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
    ofxEasyOscReceiver() : bCount(false), defaultListener(nullptr) {}
    ofxEasyOscReceiver(int portNumber) : bCount(false), defaultListener(nullptr) { receiver.setup(portNumber); }
	~ofxEasyOscReceiver() {}
	
    void setup(int portNumber) { receiver.setup(portNumber);}

    // update the receiver (look for waiting OSC messages, write the data into the variables and put the addresses into the multi-set)
    void update();
	
	// decide if you want to count incoming OSC messages
	void countIncomingMessages(bool bUse);
    // return how often you received a specific message since the last update
    int gotMessage(const string& address);
    // same as above
    int operator==(const string& address);

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
    ofxEasyOscReceiver& add(const string& address, const function<void()> & lambda);

    // register lambda function taking a single argument
    template<typename TArg>
    ofxEasyOscReceiver& add(const string& address, const function<void(TArg)> & lambda);

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

	/* set default listener */
	ofxEasyOscReceiver& setDefaultListener(void (*func)(const ofxOscMessage&));
	
	ofxEasyOscReceiver& setDefaultListener(const function<void(const ofxOscMessage&)>& lambda);
	
	template <typename TObject>
    ofxEasyOscReceiver& setDefaultListener(TObject* obj, void (TObject::*func)(const ofxOscMessage&));

	/* remove default listener */
	ofxEasyOscReceiver& removeDefaultListener();
	
protected:
    void searchAndRemove(const string& address, ofxOscListener* testobj);
    void searchAndRemoveLambdas(const string& address);

    unordered_map<string, list<unique_ptr<ofxOscListener>>> addressMap;
	unique_ptr<ofxOscListener> defaultListener;
    ofxOscReceiver receiver;
    unordered_multiset<string> incomingMessages;
    bool bCount;
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
        } else {
			// pass OSC message to default listener (if it has been set)
			if (defaultListener){
				defaultListener->dispatch(msg);
			}
		}
		
		if (bCount){
			// add address to multi-set
            incomingMessages.insert(address);
		}
    }
}

// decide if you want to count incoming OSC messages 
inline void ofxEasyOscReceiver::countIncomingMessages(bool bUse){
    bCount = bUse;
}

// return how often you received a specific message since the last update
inline int ofxEasyOscReceiver::gotMessage(const string& address){
	if (bCount){
		return incomingMessages.count(address);
	} else {
		return -1;
	} 
}

// same as above
inline int ofxEasyOscReceiver::operator==(const string& address){
    if (bCount){
		return incomingMessages.count(address);
	} else {
		return -1;
	} 
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
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address, const function<void()> & lambda){
    addressMap[address].push_back(unique_ptr<ofxOscListener>(new ofxOscLambdaFunction<void>(lambda)));
    return *this;
}
// register lambda function taking a single argument
template<typename TArg>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::add(const string& address, const function<void(TArg)> & lambda){
    addressMap[address].push_back(unique_ptr<ofxOscListener>(new ofxOscLambdaFunction<TArg>(lambda)));
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


/* set default listener */
inline ofxEasyOscReceiver& ofxEasyOscReceiver::setDefaultListener(void (*func)(const ofxOscMessage&)){
		defaultListener = unique_ptr<ofxOscListener>(new ofxOscFunction<void, const ofxOscMessage&>(func));
}

inline ofxEasyOscReceiver& ofxEasyOscReceiver::setDefaultListener(const function<void(const ofxOscMessage&)>& lambda){
	defaultListener = unique_ptr<ofxOscListener>(new ofxOscLambdaFunction<const ofxOscMessage&>(lambda));
}

template <typename TObject>
inline ofxEasyOscReceiver& ofxEasyOscReceiver::setDefaultListener(TObject* obj, void (TObject::*func)(const ofxOscMessage&)){
	defaultListener = unique_ptr<ofxOscListener>(new ofxOscMemberFunction<TObject, void, const ofxOscMessage&>(obj, func));
}

/* remove default listener */
inline ofxEasyOscReceiver& ofxEasyOscReceiver::removeDefaultListener(){
	defaultListener = nullptr;
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
