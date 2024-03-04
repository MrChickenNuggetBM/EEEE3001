#ifndef MQTTPP_HPP
#define MQTTPP_HPP

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

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <atomic>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include "mqtt/async_client.h"

namespace mqtt
{
    namespace topics
    {
        namespace cv
        {
            extern char
                threshold,
                noiseKernel,
                adaptiveSize;
        }
        namespace parameters
        {
            extern static int
                xCenter,
                yCenter,
                xDiameter,
                yDiameter,
                thickness;

            extern static bool
                isCircle,
                isGUIControl;

            extern static char
                modality;
        }

        namespace brightness
        {
            extern int dutyCycle;
            extern bool isAutomaticBrightness;
        }

    }

    /////////////////////////////////////////////////////////////////////////////

    // Callbacks for the success or failures of requested actions.
    // This could be used to initiate further action, but here we just log the
    // results to the console.

    class action_listener : public virtual iaction_listener
    {
        std::string name_;

    protected:
        void on_failure(const token &tok) override;
        void on_success(const token &tok) override;

    public:
        action_listener(const std::string &name);
    };

    class delivery_action_listener : public action_listener
    {
        std::atomic<bool> done_;
        void on_failure(const token &tok) override;
        void on_success(const token &tok) override;

    public:
        delivery_action_listener(const std::string &name);
        bool is_done() const;
    };

    /////////////////////////////////////////////////////////////////////////////

    /**
     * Local callback & listener class for use with the client connection.
     * This is primarily intended to receive messages, but it will also monitor
     * the connection to the broker. If the connection is lost, it will attempt
     * to restore the connection and re-subscribe to the topic.
     */
    class Callback : public virtual callback,
                     public virtual iaction_listener
    {
        // Counter for the number of connection retries
        int nretry_;
        // The MQTT client
        async_client &CLIENT;
        // Options to use if we need to reconnect
        connect_options &CONN_OPTS;
        // An action listener to display the result of actions.
        action_listener SUB_LISTENER;
        // array of topics to connect to
        const std::string *TOPICS;
        const char numTopics;

        // This deomonstrates manually reconnecting to the broker by calling
        // connect() again. This is a possibility for an application that keeps
        // a copy of it's original connect_options, or if the app wants to
        // reconnect with different OPTIONS.
        // Another way this can be done manually, if using the same OPTIONS, is
        // to just call the async_client::reconnect() method.
        void reconnect();

        // Re-connection failure
        void on_failure(const token &tok) override;

        // (Re)connection success
        // Either this or connected() can be used for callbacks.
        void on_success(const token &tok) override;

        // (Re)connection success
        void connected(const std::string &cause) override;

        // Callback for when the connection is lost.
        // This will initiate the attempt to manually reconnect.
        void connection_lost(const std::string &cause) override;

        // Callback for when a message arrives.
        void message_arrived(const_message_ptr msg) override;

        void delivery_complete(delivery_token_ptr token) override;

    public:
        Callback(async_client &CLIENT, connect_options &connOpts, const std::string *topics, const int numtopics);
    };

    // function to publish messages
    std::shared_ptr<delivery_token> publishMessage(std::string topic, std::string payload);

    // function to publish images
    std::shared_ptr<delivery_token> publishImage(std::string topic, cv::Mat frame);

    // declaring useful constants
    extern const string TOPICS[];
    // mqtt broker definition
    extern const string SERVER_ADDRESS;
    extern async_client CLIENT;
    // connection OPTIONS
    extern connect_options OPTIONS;
    // callback
    extern Callback CALLBACK;
}

#endif // MQTTPP_HPP
