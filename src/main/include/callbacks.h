
#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <stdint.h>
#include <utp.h>

uint64 callback_on_read(utp_callback_arguments *a);
uint64 callback_on_firewall(utp_callback_arguments *a);
uint64 callback_on_accept(utp_callback_arguments *a);
uint64 callback_on_error(utp_callback_arguments *a);
uint64 callback_on_state_change(utp_callback_arguments *a);
uint64 callback_sendto(utp_callback_arguments *a);
uint64 callback_log(utp_callback_arguments *a);

#endif