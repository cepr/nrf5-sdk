/**
  LIN Master Application
	
  Company:
    Microchip Technology Inc.

  File Name:
    lin_app.h

  Summary:
    LIN Master Application

  Description:
    This header file provides the interface between the user and 
    the LIN drivers.

 */

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

//#include "lin_master.h"

#ifndef LIN_APP_H
#define	LIN_APP_H

#include <stdint.h>
#include <stdlib.h>

extern uint8_t IGNITION_STARTER_Data[1];
extern uint8_t WIPERS_Data[1];
extern uint8_t TURN_SIGNAL_Data[1];
extern uint8_t BLINKER_Data[1];
extern uint8_t LIGHTS_Data[1];
extern uint8_t HORN_Data[1];
extern uint8_t BREAK_SWITCHES_Data[1];
extern uint8_t BACK_UP_LIGHT_Data[1];
extern uint8_t HEATER_AIR_BLOWER_Data[1];
extern uint8_t WASHER_Data[1];
extern uint8_t REAR_WINDOW_DEFOGGER_Data[1];
extern uint8_t CYLINDER_HEAD_TEMPERATURE_Data[1];
extern uint8_t FUEL_GAUGE_Data[1];
extern uint8_t BACK_UP_SWITCH_Data[1];
extern uint8_t EMERGENCY_FLASHER_SWITCH_Data[1];

void lin_app_initialize(void);

#endif	/* LIN_APP_H */

