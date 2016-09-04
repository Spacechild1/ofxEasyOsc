#include "ofxEasyOsc.h"


//*--------------------------------------------------------------------------------------------*//

/// ofxEasyOscSender

// send string message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, const string& arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    msg.addStringArg(arg);
    sender.sendMessage(msg);

    return *this;
}

// send bool message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, bool arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    msg.addIntArg(static_cast<int>(arg));
    sender.sendMessage(msg);

    return *this;
}

// send byte message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, unsigned char arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    msg.addIntArg(arg); // implicit conversion
    sender.sendMessage(msg);

    return *this;
}

// send int message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, int arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    msg.addIntArg(arg);
    sender.sendMessage(msg);

    return *this;
}

// send float message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, float arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    msg.addFloatArg(arg);
    sender.sendMessage(msg);

    return *this;
}

// send bool vector message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, const vector<bool>& arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    int length = arg.size();

    for (int i = 0; i < length; ++i){
        msg.addIntArg(static_cast<int>(arg[i]));
    }

    sender.sendMessage(msg);

    return *this;
}

// send byte vector message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, const vector<unsigned char>& arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    int length = arg.size();

    for (int i = 0; i < length; ++i){
        msg.addIntArg(arg[i]);
    }

    sender.sendMessage(msg);

    return *this;
}

// send int vector message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, const vector<int>& arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    int length = arg.size();

    for (int i = 0; i < length; ++i){
        msg.addIntArg(arg[i]);
    }

    sender.sendMessage(msg);

    return *this;
}

// send float vector message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, const vector<float>& arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    int length = arg.size();

    for (int i = 0; i < length; ++i){
        msg.addFloatArg(arg[i]);
    }

    sender.sendMessage(msg);

    return *this;
}

// send string vector message:
ofxEasyOscSender& ofxEasyOscSender::send(const string& address, const vector<string>& arg){
    ofxOscMessage msg;
    msg.setAddress(address);
    int length = arg.size();

    for (int i = 0; i < length; ++i){
        msg.addStringArg(arg[i]);
    }

    sender.sendMessage(msg);

    return *this;
}

//*----------------------------------------------------------------------------------------------------------*//
