/**
  LIN Master Driver
	
  Company:
    Microchip Technology Inc.

  File Name:
    lin_master.c

  Summary:
    LIN Master Driver

  Description:
    This source file provides the driver for LIN master nodes

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

//#include "app_error.h"

#include <stdbool.h>
#include <string.h>

#include "lin_master.h"
#include "lin_hardware.h"

#define USE_BREAK 1

typedef union {
    const lin_cmd_packet_t* cmd;
    uint8_t length;
    uint8_t data[8];
} lin_rxpacket_t;

typedef union {
    struct {
        unsigned ID0: 1;
        unsigned ID1: 1;
        unsigned ID2: 1;
        unsigned ID3: 1;
        unsigned ID4: 1;
        unsigned ID5: 1;
        unsigned P0: 1;
        unsigned P1: 1;
    };
    uint8_t rawPID;
}lin_pid_t;

static lin_rxpacket_t LIN_rxPacket;

static const lin_cmd_packet_t* schedule;
static uint8_t schedule_length;
static uint8_t schedule_index;
static const uint8_t preamble = 0x55;
static uint8_t pid;

static void send_break(void);
static void send_preamble(void);
static void send_pid(void);
static void transfer_data(void);
static void send_checksum(void);
static void timer_event(void *unused);
static void on_receive(uint8_t data);
static uint8_t calc_parity(uint8_t CMD);
static uint8_t checksum(uint8_t pid, uint8_t length, uint8_t* data);

void lin_master_init(size_t tableLength, const lin_cmd_packet_t* const table, uint32_t period_millis)
{
    schedule = table;
    schedule_length = tableLength;
    schedule_index = 0;
    lin_hardware_uart_init(19200);
    lin_hardware_timer_init(period_millis, timer_event);
}

static void send_break(void)
{
#if USE_BREAK == 1
    // Writing 0xC0 at 9600 bps is equivalent to send one start bit + 13 bits at 19200
    lin_hardware_uart_set_baudrate(9600);
    uint8_t brk = 0xc0;
    lin_hardware_uart_write(&brk, 1, send_preamble);
#else
    send_preamble();
#endif
}

static void send_preamble(void)
{
    lin_hardware_uart_set_baudrate(19200);
    lin_hardware_uart_write(&preamble, 1, send_pid);
}

static void send_pid(void)
{
    pid = calc_parity(schedule[schedule_index].cmd);
    lin_hardware_uart_write(&pid, 1, transfer_data);
}

static void transfer_data(void)
{
    if (schedule[schedule_index].type == TRANSMIT) {
        lin_hardware_uart_write(schedule[schedule_index].data, schedule[schedule_index].length, send_checksum);
    } else {
        lin_hardware_uart_enable_rx(on_receive);
    }
}

static void send_checksum(void)
{
    static uint8_t chk;
    chk = checksum(pid, schedule[schedule_index].length, schedule[schedule_index].data);
    lin_hardware_uart_write(&chk, 1, NULL);
}

static void timer_event(void *unused)
{
    // Proceed to next command in schedule table
    schedule_index = (schedule_index + 1) % schedule_length;

    // Start transmitting
    send_break();
}

static void on_receive(uint8_t data)
{
    if (LIN_rxPacket.length < schedule[schedule_index].length) {
        //save data
        LIN_rxPacket.data[LIN_rxPacket.length++] = data;
    } else {
        //calculate and verify checksum
        if(data == checksum(pid, LIN_rxPacket.length, LIN_rxPacket.data)) {
            // Valid packet, copy back into user buffer
            memcpy(LIN_rxPacket.cmd->data, LIN_rxPacket.data, LIN_rxPacket.length);
        }
    }
}

static uint8_t calc_parity(uint8_t CMD)
{
    lin_pid_t PID;
    PID.rawPID = CMD;

    //Workaround for compiler bug:
//    PID.P0 = PID.ID0 ^ PID.ID1 ^ PID.ID2 ^ PID.ID4;
//    PID.P1 = ~(PID.ID1 ^ PID.ID3 ^ PID.ID4 ^ PID.ID5);
    PID.P0 = PID.ID0 ^ PID.ID1;
    PID.P0 = PID.P0 ^ PID.ID2;
    PID.P0 = PID.P0 ^ PID.ID4;
    PID.P1 = PID.ID1 ^ PID.ID3;
    PID.P1 = PID.P1 ^ PID.ID4;
    PID.P1 = PID.P1 ^ PID.ID5;
    PID.P1 = ~PID.P1;

    return PID.rawPID;
}

static uint8_t checksum(uint8_t pid, uint8_t length, uint8_t* data)
{
    uint16_t checksum = pid;

    for (uint8_t i = 0; i < length; i++){
        checksum = checksum + *data++;
        if(checksum > 0xFF)
            checksum -= 0xFF;
    }
    checksum = ~checksum;

    return (uint8_t)checksum;
}
