// Modified from async_subscribe.cpp && async_publish.cpp
//
// This is a Paho MQTT C++ client, sample application.
//
// This application is an MQTT subscriber using the C++ asynchronous client
// interface, employing callbacks to receive messages and status updates.
//
// The sample demonstrates:
//  - Connecting to an MQTT server/broker.
//  - Subscribing to a topic
//  - Receiving messages through the callback API
//  - Receiving network disconnect updates and attempting manual reconnects.
//  - Using a "clean session" and manually re-subscribing to topics on
//    reconnect.
//

/*******************************************************************************
 * Copyright (c) 2013-2023 Frank Pagliughi <fpagliughi@mindspring.com>
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Frank Pagliughi - initial implementation and documentation
 *******************************************************************************/

#include "../include/MQTT++.h"

using namespace std;
using namespace cv;

// define some constants important for the MQTT protocol
namespace mqtt
{
const int QoS = 1,
          N_RETRY_ATTEMPTS = 5;

namespace topics
{
namespace cv
{
char
threshold = 20,
noiseKernel = 1,
adaptiveSize = 5;

float
xCorrection = 0,
yCorrection = 0,
angleCorrection = 0,
minRadCorrection = 0,
majRadCorrection = 0;

bool
isNewValues = false,
isPauseRendering = false;
}

namespace parameters
{
int xCenter = -3,
    yCenter = -1,
    xDiameter = 28,
    yDiameter = 46,
    thickness = 100;

bool isCircle = false,
     isGUIControl = false;

char modality = 0;

float angle = 0;
}

namespace brightness
{
int dutyCycle = 50;
bool isAutomaticBrightness = true;
}

}

/////////////////////////////////////////////////////////////////////////////

// Callbacks for the success or failures of requested actions.
// This could be used to initiate further action, but here we just log the
// results to the console.

void action_listener::on_failure(const token &tok)
{
    cout << name_ << " failure";
    if (tok.get_message_id() != 0)
        cout << " for token: [" << tok.get_message_id() << "]" << endl;
    cout << endl;
}

void action_listener::on_success(const token &tok)
{
    cout << name_ << " success";
    if (tok.get_message_id() != 0)
        cout << " for token: [" << tok.get_message_id() << "]" << endl;
    auto top = tok.get_topics();
    if (top && !top->empty())
        cout << "\ttoken topic: '" << (*top)[0] << "', ..." << endl;
    cout << endl;
}
action_listener::action_listener(const string &name) : name_(name) {}

void delivery_action_listener::on_failure(const token &tok)
{
    action_listener::on_failure(tok);
    done_ = true;
}

void delivery_action_listener::on_success(const token &tok)
{
    action_listener::on_success(tok);
    done_ = true;
}

delivery_action_listener::delivery_action_listener(const string &name) : action_listener(name),
    done_(false) {}

bool delivery_action_listener::is_done() const
{
    return done_;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * Local callback & listener class for use with the client connection.
 * This is primarily intended to receive messages, but it will also monitor
 * the connection to the broker. If the connection is lost, it will attempt
 * to restore the connection and re-subscribe to the topic.
 */
void Callback::reconnect()
{
    this_thread::sleep_for(chrono::milliseconds(2500));
    try
    {
        CLIENT.connect(CONN_OPTS, nullptr, *this);
    }
    catch (const mqtt::exception &exc)
    {
        cerr << "Error: " << exc.what() << endl;
        exit(1);
    }
}

// Re-connection failure
void Callback::on_failure(const token &tok)
{
    cout << "Connection attempt failed" << endl;
    if (++nretry_ > N_RETRY_ATTEMPTS)
        exit(1);
    reconnect();
}

// (Re)connection success
// Either this or connected() can be used for callbacks.
void Callback::on_success(const token &tok) {}

// (Re)connection success
void Callback::connected(const string &cause)
{
    cout << "\nConnection success" << endl
         << endl;

    int i = 0;
    for (string topic = TOPICS[i]; i < numTopics; topic = TOPICS[++i])
    {
        cout << "Subscribing to topic '" << topic << "'" << endl
             << "\tfor client " << CLIENT.get_client_id() << " using QoS" << QoS << endl
             << endl
             << flush;

        CLIENT.subscribe(topic, QoS, nullptr, SUB_LISTENER);
    }
}

// Callback for when the connection is lost.
// This will initiate the attempt to manually reconnect.
void Callback::connection_lost(const string &cause)
{
    cout << "\nConnection lost" << endl;
    if (!cause.empty())
        cout << "\tcause: " << cause << endl;

    cout << "Reconnecting..." << endl;
    nretry_ = 0;
    reconnect();
}

void Callback::delivery_complete(delivery_token_ptr token)
{
    if (!token)
    {
        // cout << "Delivery complete for token: -1"
        //           << endl << flush;
        return;
    }

    // cout << "Delivery complete for token: " << token->get_message_id()
    //           << endl << flush;

    return;
}

Callback::Callback(async_client &CLIENT, connect_options &connOpts, const string *topics, const int numtopics)
    : nretry_(0),
      CLIENT(CLIENT),
      CONN_OPTS(connOpts),
      SUB_LISTENER("Subscription"),
      TOPICS(topics),
      numTopics(numtopics) {}

// function to publish messages
shared_ptr<delivery_token> publishMessage(string topic, string payload)
{
    const char *_topic = topic.data();
    const char *_payload = payload.data();

    auto token = CLIENT.publish(_topic, _payload, strlen(_payload), QoS, false);

    // cout << endl << "Delivering: " << _topic << " = " << _payload << " [" << token->get_message_id() << "]"
    // << endl << flush;
    // << endl << flush;

    return token;
}

// function to publish images
shared_ptr<delivery_token> publishImage(string topic, cv::Mat frame)
{
    const char *_topic = topic.data();

    vector<uchar> frameBytes;
    cv::imencode(".jpg", frame, frameBytes);
    auto msg = make_message(_topic, frameBytes.data(), frameBytes.size());
    auto token = CLIENT.publish(msg);

    // cout << endl << "Delivering: " << _topic << " [" << token->get_message_id() << "]"
    // << endl << flush;

    return token;
}
}
