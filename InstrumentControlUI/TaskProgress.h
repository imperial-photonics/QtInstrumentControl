#pragma once

#include <QObject>
#include <memory>
#include <mutex>
#include <list>


class TaskProgress : public QObject
{
   Q_OBJECT

public:
   TaskProgress(const QString& task_name, bool cancellable = false, int n_steps = 0, QObject* parent = nullptr) :
      QObject(parent), cancellable(cancellable), task_name(task_name), n_steps(n_steps)
   {

   }

   void incrementStep()
   {
      cur_step++;
      setProgress((double)(cur_step) / n_steps);
   }

   void setProgress(double progress_)
   {
      progress = progress_;
      emit progressUpdated(progress);
      emit progressUpdatedPercentage(progress * 100);
   }

   void setFinished()
   {
      if (!finished)
         emit taskFinished();
      finished = true;
   }

   void requestCancel()
   {
      emit cancelRequested();
      cancel_requested = true;
   }

   void setTaskName(const QString& task_name_)
   {
      task_name = task_name_;
      emit taskNameChanged(task_name);
   }

   bool wasCancelRequested() { return cancel_requested; }
   bool isIndeterminate() { return (n_steps == 0) && (progress == 0); }
   bool isCancellable() { return cancellable; }
   double getProgress() { return progress; }
   bool isFinished() { return finished; }
   const QString& getName() { return task_name; }

signals:

   void progressUpdated(double progress);
   void progressUpdatedPercentage(int progress); // for easy connection to progressbar
   void taskFinished();
   void cancelRequested();
   void taskNameChanged(const QString& task_name);

private:

   bool cancellable = false;
   bool cancel_requested = false;
   int cur_step = 0;
   int n_steps = 0;
   double progress = 0;
   bool finished = false;
   QString task_name;

};


class TaskRegister : public QObject
{
   Q_OBJECT

public:

   static TaskRegister* getRegister()
   {
      if (!instance)
         instance = new TaskRegister;
      
      return instance;
   }

   static void addTask(const std::shared_ptr<TaskProgress>& task)
   {
      getRegister()->addTaskImpl(task);
   }

   const std::list<std::shared_ptr<TaskProgress>>& getTasks()
   {
      std::lock_guard<std::mutex> lk(task_mutex);
      return tasks;
   }

signals:

   void newTaskAdded();

private:

   void addTaskImpl(const std::shared_ptr<TaskProgress>& task)
   {
      {
         std::lock_guard<std::mutex> lk(task_mutex);
         tasks.push_back(task);
      }
      connect(task.get(), &TaskProgress::taskFinished, this, &TaskRegister::cleanupTasks, Qt::QueuedConnection);
      emit newTaskAdded();
   }

   void cleanupTasks()
   {
      // Remove completed tasks from the register
      std::lock_guard<std::mutex> lk(task_mutex);
      tasks.remove_if([](const std::shared_ptr<TaskProgress>& task) { return task->isFinished(); });
   }
  
   TaskRegister() {};

   std::mutex task_mutex;
   std::list<std::shared_ptr<TaskProgress>> tasks;
   static TaskRegister* instance;
};
