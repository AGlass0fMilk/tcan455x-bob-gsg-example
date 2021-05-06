/*
 * TCAN455x Breakout Board Getting Started Guide Example
 * Copyright (c) 2020 George Beckstein
 * SPDX-License-Identifier: Apache-2.0
 *
 * Build with ARM Mbed-OS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

#include "TCAN455x.h"

#include "rtos/ThisThread.h"
#include "events/EventQueue.h"
#include "platform/Callback.h"

#include <chrono>

using namespace std::chrono;

mbed::DigitalOut can1_rst(P0_31, 1);
TCAN455x can1(P0_29, P0_28, P0_30, P0_4, P0_3, &can1_rst);
mbed::DigitalOut led1(LED1);

/**
 * If you're not using the second TCAN455x-BoB, you can disable this in
 * the mbed_app.json configuration file.
 *
 * If disabled, this demo will just listen to the CAN bus and write packets
 * to the trace log.
 */
#if MBED_CONF_APP_HAS_TWO_TCANS
mbed::DigitalOut can2_rst(P0_23, 1);
TCAN455x can2(P0_26, P0_2, P0_27, P0_25, P0_24, &can2_rst);
mbed::DigitalOut led2(LED2);
#endif

char counter = 0;

#if MBED_CONF_APP_HAS_TWO_TCANS
void send()
{
    if (can2.write(mbed::CANMessage(1337, &counter, 1))) {
        counter++;
        printf("message sent: %d\n", counter);
    }
    led2 = !led2;
}
#endif

void receive()
{
    mbed::CANMessage msg;
    if (can1.read(msg)) {
        printf("message received: %d\n", msg.data[0]);
        led1 = !led1;
    }
}

int main()
{
    events::EventQueue queue;
    printf("TCAN455x BoB Getting Started Guide Example\n");

    /* Important, you must initialize the TCAN455x driver */
    can1.init();
//    can1.filter(1337, 0x7FF, CANStandard);

#if MBED_CONF_APP_HAS_TWO_TCANS
    can2.init();
#endif

    /* Send a packet every 1 second, check for new packets every 200ms */
#if MBED_CONF_APP_HAS_TWO_TCANS
    queue.call_every(1s, mbed::callback(send));
#endif

    queue.call_every(1s, mbed::callback(receive));

    queue.dispatch_forever();

    while (1) {
    }

    return 0;
}
