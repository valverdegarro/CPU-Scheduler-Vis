#ifndef EVENTS_H
#define EVENTS_H

/* PRIVATE functions */

int get_esem_wait_time();

int send_event(event_t *event);

int init_events(event_t *event_mem, buff_msg *buffer);

int attach_events(event_t *event_mem, buff_msg *buffer);

#endif // EVENTS_H