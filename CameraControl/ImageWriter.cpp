#include "ImageWriter.h"


ImageWriter::ImageWriter(ImageSource* camera, QObject* parent, QThread* thread) :
ThreadedObject(parent, thread),
camera(camera)
{
   file_root = "camera ";
   active = false;

   StartThread();
}

void ImageWriter::Init()
{
   connect(camera, &ImageSource::NewImage, this, &ImageWriter::ImageUpdated, Qt::QueuedConnection);
};

void ImageWriter::SetFilenameRoot(const QString& file_root_)
{
   file_root = file_root_;
   QString cf = folder;
   complete_file_root = cf.append("/").append(file_root).toStdString();
   emit FilenameRootChanged(file_root_);
}

void ImageWriter::ChooseFolder()
{
   folder = QFileDialog::getExistingDirectory(0, tr("Choose folder"), "R:/");
   QString cf = folder;
   complete_file_root = cf.append("/").append(file_root).toStdString(); 
   emit FolderChanged(folder);
}

void ImageWriter::SetFolder(const QString& folder_)
{
   folder = folder_;
   emit FolderChanged(folder);
}

void ImageWriter::SetBufferSize(int buffer_size_)
{
   if (!active && buffer_size_ > 0)
   {
      buffer_size = buffer_size_;
      InitBuffer();

      emit BufferSizeChanged(buffer_size);
   }
  
}


void ImageWriter::SetActive(bool active_)
{
   // If we're stopping early
   if (active & !active_)
   {
      active = false;
      WriteBuffer();
   }
      
   if (active_ && folder.isEmpty())
      GetFolder();

   if (active_)
   {
      InitBuffer();
      camera->SetImageProductionStatus(true);
   }

   active = active_;
   file_idx = 0;

   emit ActiveStateChanged(active);
}

void ImageWriter::InitBuffer()
{
   cv::Mat& image = camera->GetImage();

   cv::Size sz = image.size();
   int type = image.type();

   if ((buffer_image_size != sz) || (buffer_image_type != type) || (buffer_size != buffer.size()))
   {
      buffer.empty();
      buffer.resize(buffer_size, cv::Mat(sz, type));
   }
}

void ImageWriter::SaveSingle()
{
   QString filename = QFileDialog::getSaveFileName(0, "Choose File Name", folder, "tif file (*.tif)");

   if (filename.isEmpty())
      return;

   cv::Mat m = camera->GetImage();

   // discard unused alpha channel
   if (m.type() == CV_8UC4)
      cv::cvtColor(m, m, CV_BGRA2BGR);


   cv::imwrite(filename.toStdString(), m);
}

void ImageWriter::ImageUpdated()
{
   if (active)
   {
      cv::Mat m = camera->GetImageUnsafe();
      m.copyTo(buffer[file_idx]);

      file_idx++;

      emit ProgressUpdated((100.0 * file_idx) / buffer.size());

      if (file_idx == buffer.size())
      {
         emit ProgressUpdated(100);
         active = false;
         emit ActiveStateChanged(active);
         WriteBuffer();
         camera->SetImageProductionStatus(false);
      }

   }

}

void ImageWriter::WriteBuffer()
{
   emit EnabledStateChanged(false);
   for (int i = 0; i < file_idx; i++)
   {
      std::stringstream filename;
      filename << complete_file_root << std::setw(5) << std::setfill('0') << i << ".tif";

      // discard unused alpha channel
      if (buffer[i].type() == CV_8UC4)
         cv::cvtColor(buffer[i], buffer[i], CV_BGRA2BGR);

      cv::imwrite(filename.str(), buffer[i]);
   }
   emit ProgressUpdated(0);
   emit EnabledStateChanged(true);
}
