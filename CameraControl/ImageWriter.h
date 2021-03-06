#pragma once
#include <QFileDialog>
#include "ThreadedObject.h"
#include "ImageSource.h"

#include <string>

#include <cv.h>
#include <opencv2/highgui/highgui.hpp>

class ImageWriter : public ThreadedObject
{
   Q_OBJECT

public:
   ImageWriter(ImageSource* camera, QObject* parent = 0, QThread* thread = 0);

   void init();
   void SaveSingle();

   void SetFilenameRoot(const QString& file_root_);
   void SetFolder(const QString& folder_);
   
   void ChooseFolder();

   const QString& GetFilenameRoot() { return file_root; }
   const QString& GetFolder() { return folder; }

   void SetBufferSize(int buffer_size_);
   int GetBufferSize() { return buffer_size; }


   void SetActive(bool active_);

   void ImageUpdated();


signals:
   void FolderChanged(const QString& folder);
   void FilenameRootChanged(const QString& filename_root);
   void BufferSizeChanged(int buffer_size);
   void ActiveStateChanged(bool active);
   void ProgressUpdated(int progress);
   void EnabledStateChanged(bool enabled);

private:

   void WriteBuffer();
   void InitBuffer();


   ImageSource* camera;
   bool active;
   int file_idx;
   std::string complete_file_root;
   QString file_root;
   QString folder;
   QThread worker_thread;

   std::vector<cv::Mat> buffer;

   cv::Size buffer_image_size;
   int buffer_image_type = CV_16U;

   int buffer_size = 100;
};
