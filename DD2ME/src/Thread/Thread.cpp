#include "Thread.h"

namespace ext {

static void ThreadCallback(Thread* who) {
#ifndef WIN32
  // Далаем поток "убиваемым" через pthread_cancel.
  int old_thread_type;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &old_thread_type);
#endif
  who->Execute();
}

#ifdef WIN32

Thread::~Thread() {
  CloseHandle(__handle);
}

void Thread::Start() {
  __handle = CreateThread(
    0, 0,
    reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadCallback), this,
    0, 0
  );
}

void Thread::Join() {
  WaitForSingleObject(__handle,  INFINITE);
}

void Thread::Kill() {
  TerminateThread(__handle, 0);
}

#else

Thread::~Thread() {
}

extern "C"
typedef void *(*pthread_callback)(void *);

void Thread::Start() {
  pthread_create(
    &__handle, 0,
    reinterpret_cast<pthread_callback>(ThreadCallback),
    this
  );
}

void Thread::Join() {
  pthread_join(__handle, 0);
}

void Thread::Kill() {
  pthread_cancel(__handle);
}

#endif

} // ext
