#pragma once

#include <QObject>
#include <ImageSource.h>
#include <thread>
#include <functional>

#include "ImageRenderWindow.h"

class ImageSeriesScanner : public QObject
{
   Q_OBJECT

public:

   ImageSeriesScanner(std::vector<ImageSource*> image_sources, QObject* parent = 0) :
      QObject(parent),
      image_sources(image_sources)
   {
   }

   template<class T>
   void SetPositionController(T* position_controller, void (T::*setter_)(double), double (T::*getter_)(), const QString& value_name_ = "P", const QString unit_ = "")
   {
      SetPosition = std::bind(setter_, position_controller, std::placeholders::_1);
      GetPosition = std::bind(getter_, position_controller);

      value_name = value_name_;
      unit = unit_;
   }

   void SetScanStart(double scan_start_) { scan_start = scan_start_; }
   double GetScanStart() { return scan_start; }

   void SetScanEnd(double scan_end_) { scan_end = scan_end_; }
   double GetScanEnd() { return scan_end; }

   void SetNSteps(int n_steps_) { n_steps = n_steps_; }
   int GetNSteps() { return n_steps; }

   const QString& GetUnits() { return unit; }

   void SetImageSourceIndex(unsigned int image_source_index_)
   {
      if (image_source_index >= image_sources.size())
         return;

      image_source_index = image_source_index_;
   }

   unsigned int GetImageSourceIndex() { return image_source_index; }


   void SetScanning(bool scanning_)
   {
      if (scanning_ && !worker.joinable())
      {
         // Create a new window for display
         QString title = value_name;
         title.append(" Scan");

         ImageRenderWindow* window = new ImageRenderWindow(title);
         ImageRenderWidget* render_widget = window->GetRenderWidget();
         window->show();
         emit NewRenderWindow(window);

         terminate = false;
         worker = std::thread(&ImageSeriesScanner::Scan, this, render_widget);
      }

      if (!scanning_ && worker.joinable())
      {
         terminate = true;
         worker.join();
      }
   }

   void Scan(ImageRenderWidget* render_widget)
   {
      double start = scan_start;
      double end = scan_end;
      double n = n_steps;
      double step = (end - start) / (n - 1);

      for (int i = 0; i < n; i++)
      {

         float position = start + i*step;
         SetPosition(position);
         // slm->WaitForNextUpdate(); // TODO

         cv::Mat m = image_sources[image_source_index]->GetNextImage();
         render_widget->AddImage(m, QString("%1=%2%3").arg(value_name).arg(position, 0, 'f', 3).arg(unit));

         emit ProgressChanged((100 * (i + 1)) / n);

         if (terminate)
            break;
      }

      emit ScanningChanged(false);

   }

signals:

   void ProgressChanged(int percentage_progress);
   void NewRenderWindow(ImageRenderWindow* image_render_window);
   void ScanningChanged(bool scanning);
   void NewImage(cv::Mat image, QString label);

private:

   double scan_start;
   double scan_end;
   int n_steps;

   unsigned int image_source_index = 0;
   std::vector<ImageSource*> image_sources;

   std::thread worker;
   bool terminate = true;

   QString value_name = "P";
   QString unit = "";

   std::function<void(double)> SetPosition;
   std::function<double(void)> GetPosition;
};