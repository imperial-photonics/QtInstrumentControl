#pragma once

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

 
class ThreadedObject : public QObject
{
   Q_OBJECT
public:
   ThreadedObject(QObject* parent = nullptr, QThread* ex_thread = nullptr);
   virtual ~ThreadedObject();

   void startThread();
   QThread* getThread();
   void parentDestroyed();

   virtual void init() = 0;

signals:
   void started();

protected:

   Q_INVOKABLE void startInit();

   QThread* thread;
   bool private_thread;

   QMutex init_mutex;
   QWaitCondition init_cv;
};

