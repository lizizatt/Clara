// (c)2014-2015, Jaunt Inc. Adam Somers, Arthur van Hoff

#ifndef __Jaunt_MessageListenerList__
#define __Jaunt_MessageListenerList__

#include "../JuceLibraryCode/JuceHeader.h"

//
// Broadcast a message to a multiple listeners.
//
class MessageListenerList : private MessageListener
{
    JUCE_LEAK_DETECTOR(MessageListenerList)
private:
    CriticalSection mutex;
    Array<MessageListener *> listeners;

public:
    //enum NoticeLevel {INFO, WARNING, ERROR};
    enum NoticeLevel {LEVEL_INFO, LEVEL_WARNING, LEVEL_ERROR};

    class Notice : public Message {
    public:
        int64 when;
        NoticeLevel level;
        String msg;
    public:
    Notice(NoticeLevel level, const String& msg) : when(Time::currentTimeMillis()), level(level), msg(msg) {}
    };

public:
    void addListener(MessageListener *listener);
    void removeListener(MessageListener *listener);
    void removeAllListeners();
    void postMessage(Message *msg);

    void postNotice(MessageListenerList::NoticeLevel level, const char *fmt, va_list &args) __attribute__((format(printf, 3, 0)));
    void postWarning(const char *fmt, ...) __attribute__((format(printf, 2, 3)));
    void postError(const char *fmt, ...) __attribute__((format(printf, 2, 3)));
    void postInfo(const char *fmt, ...) __attribute__((format(printf, 2, 3)));
   
protected:
    virtual void handleMessage(const Message& msg) override;
};

#endif
