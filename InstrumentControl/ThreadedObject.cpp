#include "ThreadedObject.h"

ThreadedObject::ThreadedObject(QObject* parent, QThread* ex_thread)
   : thread(ex_thread)
{

   if (parent != nullptr)
      connect(parent, &QObject::destroyed, this, &ThreadedObject::deleteLater);

   private_thread = (thread == nullptr);
   if (thread == nullptr)
      thread = new QThread;

   this->moveToThread(thread);

   // Start thread if it isn't running
   if (!thread->isRunning())
      thread->start();

   if (private_thread)
   {
      connect(this, &QObject::destroyed, thread, &QThread::quit, Qt::QueuedConnection);
      connect(thread, &QThread::finished, thread, &QThread::deleteLater, Qt::QueuedConnection);
   }

   //connect(this, &ThreadedObject::Started, this, &ThreadedObject::startInit, Qt::QueuedConnection);
   //emit Started();
}

void ThreadedObject::startInit()
{
   QMutexLocker lk(&init_mutex);
   this->init();
   init_cv.wakeAll();
}

void ThreadedObject::startThread()
{
   thread->setObjectName(objectName());
   
   QMutexLocker lk(&init_mutex);

   // Call Init function
   QMetaObject::invokeMethod(this, "startInit", Qt::QueuedConnection);
   init_cv.wait(&init_mutex);
}

ThreadedObject::~ThreadedObject()
{
}