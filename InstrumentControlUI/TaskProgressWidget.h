#pragma once

#include <QWidget>
#include <QLayout>
#include "ui_TaskProgressWidget.h"

#include <memory>
#include <map>
#include "TaskProgress.h"

class TaskProgressWidget : public QWidget, Ui::TaskProgressWidget
{
   Q_OBJECT

public:
   TaskProgressWidget(QWidget* parent = nullptr) : 
      QWidget(parent)
   {
      setupUi(this);
   }

   void setTask(std::shared_ptr<TaskProgress> task_)
   {
      task = task_;

      label->setText(task->getName());
      if (!task->isIndeterminate())
         progress_bar->setMaximum(100);
      
      connect(task.get(), &TaskProgress::taskFinished, [&]() { deleteLater(); });
      connect(task.get(), &TaskProgress::progressUpdatedPercentage, progress_bar, &QProgressBar::setValue);
      connect(cancel_button, &QPushButton::pressed, task.get(), &TaskProgress::requestCancel);
     // connect(cancel_button, &QPushButton::pressed, cancel_button, &QPushButton::setDisabled);
   }

protected:

   std::shared_ptr<TaskProgress> task;
};

class ProgressCentre : public QWidget
{
   Q_OBJECT

public:
   ProgressCentre(QWidget* parent = nullptr) :
      QWidget(parent)
   {
      task_register = TaskRegister::getRegister();
      layout = new QVBoxLayout;
      layout->setMargin(0);

      setLayout(layout);

      connect(task_register, &TaskRegister::newTaskAdded, this, &ProgressCentre::update);
   }

   void update()
   {
      auto tasks = task_register->getTasks();
      for (auto& task : tasks)
      {
         TaskProgressWidget* w;
         auto it = task_map.find(task.get());

         if (it == task_map.end())
         {
            w = new TaskProgressWidget;
            task_map[task.get()] = w;
            w->setTask(task);
            layout->addWidget(w);
         }
         else
         {
            w = it->second;
         }
      }
   }

   QVBoxLayout *layout;

   TaskRegister* task_register;
   std::map<TaskProgress*, TaskProgressWidget*> task_map;
};