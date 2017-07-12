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
         setProgressBarDeterminate();

      cancel_button->setVisible(task->isCancellable());      
      connect(task.get(), &TaskProgress::taskFinished, [&]() { deleteLater(); });
      connect(task.get(), &TaskProgress::progressUpdatedPercentage, progress_bar, &QProgressBar::setValue);
      connect(task.get(), &TaskProgress::progressUpdatedPercentage, this, &TaskProgressWidget::setProgressBarDeterminate);
      connect(task.get(), &TaskProgress::taskNameChanged, label, &QLabel::setText);
      connect(cancel_button, &QPushButton::pressed, this, &TaskProgressWidget::cancelPressed);
   }

   void cancelPressed()
   {
      task->requestCancel();
      task->setTaskName("Cancelling...");
      progress_bar->setMinimum(0);
      progress_bar->setMaximum(0);
      progress_bar->setValue(0);
      cancel_button->setEnabled(false);
   }

   void setProgressBarDeterminate()
   {
      progress_bar->setMaximum(100);
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