// Controller for machine that rips fabric. A spool runs continuously but slowly
// while a guide moves every now and then to collect the ripped fabric on a
// different part of the spool.
//
// Author: Otto Urpelainen
// Email: oturpe@iki.fi
// Date: 2016-06-21

#include "Atmega8Utils.h"

#include "config.h"

#include <avr/io.h>
#include <util/delay.h>

/// \brief
///    Toggles the indicator led state.
void toggleIndicator() {
    static bool lit;

    if (lit) {
        INDICATOR_DATA &= ~BV(INDICATOR_DATA_PIN);
    }
    else {
        INDICATOR_DATA |= BV(INDICATOR_DATA_PIN);
    }

    lit = !lit;
}

/// \brief
///    Sets the motors to the state they should have on startup.
///
/// Motor 1 is always running, motor 2 is also initially running
void initializeMotors() {
    // Set running forwards
    MOTOR_1_FORWARD_OUTPUT_COMPARE = SPOOL_SPEED;
    MOTOR_1_REVERSE_OUTPUT_COMPARE = 0x00;

    MOTOR_2_FORWARD_OUTPUT_COMPARE = GUIDE_SPEED;
    MOTOR_2_REVERSE_OUTPUT_COMPARE = 0x00;

    // Enable all outputs
    MOTOR_1_FORWARD_DATA |= BV(MOTOR_1_FORWARD_DATA_PIN);
    MOTOR_2_FORWARD_DATA |= BV(MOTOR_2_FORWARD_DATA_PIN);
    MOTOR_1_REVERSE_DATA |= BV(MOTOR_1_REVERSE_DATA_PIN);
    MOTOR_2_REVERSE_DATA |= BV(MOTOR_2_REVERSE_DATA_PIN);
}

/// \brief
///    Calculates the correct acceleration to use when current and target speed
///    are as given.
int16_t acceleration(uint8_t current, uint8_t target, uint8_t maxAcceleration) {
    int16_t acceleration;

    acceleration = target - current;

    if (acceleration > maxAcceleration) {
        acceleration = maxAcceleration;
    }

    return acceleration;
}

/// \brief
///    Changes motor 1 speed according to given speed target and maximum
///    acceleration.
///
/// \param forwardTarget
///    Forward target speed
///
/// \param reverseTarget
///    Reverse target speed
///
/// \param acceleration
///    Desired acceleration
void updateMotor1Speed(
    uint8_t forwardTarget,
    uint8_t reverseTarget,
    uint8_t acceleration
) {
    if (!MOTOR_1_REVERSE_OUTPUT_COMPARE) {
        MOTOR_1_FORWARD_OUTPUT_COMPARE += ::acceleration(
            MOTOR_1_FORWARD_OUTPUT_COMPARE,
            forwardTarget,
            acceleration
        );
    }
    if (!MOTOR_1_FORWARD_OUTPUT_COMPARE) {
        MOTOR_1_REVERSE_OUTPUT_COMPARE += ::acceleration(
            MOTOR_1_REVERSE_OUTPUT_COMPARE,
            reverseTarget,
            acceleration
        );
    }
}

/// \brief
///    Changes motor 2 speed according to given speed target and maximum
///    acceleration.
///
/// See updateMotor1Speed() function documentation for details. This function
/// behaves identically to that one.
void updateMotor2Speed(
    uint8_t forwardTarget,
    uint8_t reverseTarget,
    uint8_t acceleration
) {
    if (!MOTOR_2_REVERSE_OUTPUT_COMPARE) {
        MOTOR_2_FORWARD_OUTPUT_COMPARE += ::acceleration(
            MOTOR_2_FORWARD_OUTPUT_COMPARE,
            forwardTarget,
            acceleration
        );
    }
    if (!MOTOR_2_FORWARD_OUTPUT_COMPARE) {
        MOTOR_2_REVERSE_OUTPUT_COMPARE += ::acceleration(
            MOTOR_2_REVERSE_OUTPUT_COMPARE,
            reverseTarget,
            acceleration
        );
    }
}

/// \brief
///    Sets spool motor (1) rotation according to enable input value.
///
/// \param enable
///    If output is enabled
void controlSpool(bool enable) {
    uint8_t forwardTarget = enable ? GUIDE_SPEED : 0x00;

    updateMotor1Speed(forwardTarget, 0x00, GUIDE_ACCELERATION);
}

/// \brief
///    Sets guide motor (2) rotation according to a slow predefined duty cycle
///    and an enable input.
///
/// The duty cycle implemented within this function is running continuously. The
/// motor is stopped if it is either off phase of the cycle, of enable input is
/// not set.
///
/// \param enable
///    If output is enabled
void controlGuide(bool enable) {
    static uint8_t forwardTarget = GUIDE_SPEED;
    static uint16_t counter = 0;

    counter++;

    if (counter == GUIDE_ON_PERIOD) {
        forwardTarget = 0x00;
        counter++;
    }
    else if (counter == GUIDE_ON_PERIOD + GUIDE_OFF_PERIOD) {
        // Start to run immediately
        forwardTarget= GUIDE_SPEED;
        // Start from beginning of sequence
        counter = 0;
    }

    uint8_t adjustedTarget = enable ? GUIDE_SPEED : 0x00;
    updateMotor2Speed(adjustedTarget, 0x00, GUIDE_ACCELERATION);
}

int main() {
    INDICATOR_DATA_DIR |= BV(INDICATOR_DATA_DIR_PIN);

    MOTOR_1_FORWARD_DATA_DIR |= BV(MOTOR_1_FORWARD_DATA_DIR_PIN);
    MOTOR_1_REVERSE_DATA_DIR |= BV(MOTOR_1_REVERSE_DATA_DIR_PIN);
    MOTOR_2_FORWARD_DATA_DIR |= BV(MOTOR_2_FORWARD_DATA_DIR_PIN);
    MOTOR_2_REVERSE_DATA_DIR |= BV(MOTOR_2_REVERSE_DATA_DIR_PIN);

    Atmega8::initializeTimer0(
        PRESCALER_VALUE,
        Atmega8::PWM_PHASE_CORRECT,
        Atmega8::TOP_00FF
    );
    Atmega8::initializeTimer1(
        PRESCALER_VALUE,
        Atmega8::PWM_PHASE_CORRECT,
        Atmega8::TOP_00FF
    );
    Atmega8::initializeTimer2(
        PRESCALER_VALUE,
        Atmega8::PWM_PHASE_CORRECT,
        Atmega8::TOP_00FF
    );

    OCR1AH = 0x00;
    OCR1BH = 0x00;

    // Set non-inverting pwm
    TCCR0A |= BV(COM0A1) | BV(COM0B1);
    TCCR1A |= BV(COM1A1) | BV(COM1B1);
    TCCR2A |= BV(COM2A1) | BV(COM2B1);

    initializeMotors();

    uint16_t indicatorCounter = 0;
    uint16_t enableCounter = 0;

    while (true) {
        if (indicatorCounter == INDICATOR_HALF_PERIOD) {
            toggleIndicator();
            indicatorCounter == 0;
        }
        else {
            indicatorCounter++;
        }

        bool enable = false;
        if (enableCounter < ENABLE_ON_PERIOD) {
            enable = true;
            enableCounter++;
        }
        else if (enableCounter == ENABLE_ON_PERIOD + ENABLE_OFF_PERIOD) {
            enableCounter = 0;
        }
        else {
            enableCounter++;
        }

        controlSpool(enable);
        controlGuide(enable);

        _delay_ms(LOOP_DELAY);
    }
}
