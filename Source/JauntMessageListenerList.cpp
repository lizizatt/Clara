// (c)2014-2015, Jaunt Inc. Adam Somers, Arthur van Hoff

#include "JauntMessageListenerList.h"

void MessageListenerList::addListener(MessageListener *listener)
{
    ScopedLock lock(mutex);
    for (int i = 0 ; i < listeners.size() ; i++) {
        if (listeners[i] == listener) {
            printf("warning: listener added twice\n");
            return;
        }
    }
    listeners.add(listener);
}

void MessageListenerList::removeListener(MessageListener *listener)
{
    ScopedLock lock(mutex);
    for (int i = 0 ; i < listeners.size() ; i++) {
        if (listeners[i] == listener) {
            listeners.remove(i);
            break;
        }
    }
}

void MessageListenerList::removeAllListeners()
{
    ScopedLock lock(mutex);
    listeners.clear();
}

void MessageListenerList::postMessage(Message *msg)
{
    MessageListener::postMessage(msg);
}

void MessageListenerList::handleMessage(const Message& msg)
{
    ScopedLock lock(mutex);
    for (int i = 0 ; i < listeners.size() ; i++) {
        listeners[i]->handleMessage(msg);
    }
}

void MessageListenerList::postNotice(MessageListenerList::NoticeLevel level, const char *fmt, va_list &args)
{
    char buf[4096];
    vsnprintf(buf, sizeof(buf), fmt, args);
    postMessage(new Notice(level, CharPointer_UTF8(buf)));
}

void MessageListenerList::postWarning(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    postNotice(LEVEL_WARNING, fmt, args);
    va_end(args);
}

void MessageListenerList::postError(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    postNotice(LEVEL_ERROR, fmt, args);
    va_end(args);
}

void MessageListenerList::postInfo(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    postNotice(LEVEL_INFO, fmt, args);
    va_end(args);
}
