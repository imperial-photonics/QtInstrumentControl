#pragma once

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

 
class ThreadedObject : public QObject
{
   Q_OBJECT
public:
   ThreadedObject(QObject* parent = nullptr, QThread* ex_thread = nullptr);

   void StartThread();

   QThread* GetThread();

   void ParentDestroyed();

   virtual ~ThreadedObject();

   virtual void Init() = 0;

signals:
   void Started();

protected:

   Q_INVOKABLE void StartInit();

   QThread* thread;
   bool private_thread;

   QMutex init_mutex;
   QWaitCondition init_cv;
};

