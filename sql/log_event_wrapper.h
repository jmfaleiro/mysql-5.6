#ifndef LOG_EVENT_WRAPPER_H
#define LOG_EVENT_WRAPPER_H

#include "log_event.h"

int slave_worker_exec_job(Slave_worker *worker, Relay_log_info *rli);

/**
  @class Log_event_wrapper
  */
class Log_event_wrapper
{
  Log_event *event;
  Log_event_wrapper *begin_event;

 // Condition and lock for when the event is ready to be executed
  mysql_cond_t cond;
  mysql_mutex_t mutex;

  bool ready_to_execute;

public:
  /* JMF: debugging */
  Log_event_wrapper *link_prev;
  Log_event_wrapper *link_next;

  std::atomic<uint> is_assigned; // has this event been assigned to a worker?
  bool is_appended_to_queue; // has this event been assigned to a worker queue?
  bool is_begin_event;
  bool is_end_event;
  bool whole_group_in_dag; // entire group of this event exists in the DAG?

  Log_event_wrapper(Log_event *event, Log_event_wrapper *begin_event)
  {
    this->event= event;
    this->begin_event= begin_event;
    ready_to_execute= false;
    whole_group_in_dag= false;
    is_assigned.store(0U);
    is_appended_to_queue= false;
    is_begin_event= false;
    is_end_event= false;
    mysql_mutex_init(0, &mutex, MY_MUTEX_INIT_FAST);
    mysql_cond_init(0, &cond, NULL);
  }

  ~Log_event_wrapper()
  {
    // case: event was not appended to a worker's queue, so we need to delete it
    if (!is_appended_to_queue)
      delete event;
    mysql_mutex_destroy(&mutex);
    mysql_cond_destroy(&cond);
  }

  inline Log_event* get_raw_event() const
  {
    return event;
  }

  inline void set_raw_event(Log_event *ev)
  {
    event= ev;
  }

  inline Log_event_wrapper* get_begin_event() const
  {
    return begin_event;
  }

  inline void wait()
  {
    mysql_mutex_lock(&mutex);
    while (!ready_to_execute)
      mysql_cond_wait(&cond, &mutex);
    mysql_mutex_unlock(&mutex);
  }

  inline void signal()
  {
    mysql_mutex_lock(&mutex);
    ready_to_execute= true;
    mysql_cond_signal(&cond);
    mysql_mutex_unlock(&mutex);
  }

  inline int execute(Slave_worker *w, THD *thd, Relay_log_info *rli)
  {
    // the raw event was already added to the worker's jobs queue, so it's safe
    // to call @slave_worker_exec_job function directly
    return slave_worker_exec_job(w, rli);
  }
};

#endif // LOG_EVENT_WRAPPER_H
