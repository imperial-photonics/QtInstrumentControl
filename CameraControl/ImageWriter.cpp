#include "ImageWriter.h"

using std::shared_ptr;

ImageWriter::ImageWriter(AbstractStreamingCamera* camera, QObject* parent) :
QObject(parent), camera(camera)
{
   file_root = "camera ";
   active = false;
   connect(camera, &AbstractStreamingCamera::NewImage, this, &ImageWriter::ImageUpdated, Qt::QueuedConnection);

   worker_thread.start();
   moveToThread(&worker_thread);
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
   if (!active && buffer_size > 0)
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
      InitBuffer();

   active = active_;
   file_idx = 0;

   emit ActiveStateChanged(active);
}

void ImageWriter::InitBuffer()
{
   shared_ptr<Buf> buf = camera->GetLatest();
   cv::Mat& image = buf->GetImage();

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

   shared_ptr<Buf> buf = camera->GetLatest();
   cv::imwrite(filename.toStdString(), buf->GetBackgroundSubtractedImage());
}

void ImageWriter::ImageUpdated()
{
   if (active)
   {
      shared_ptr<Buf> buf = camera->GetLatest();

      buf->GetBackgroundSubtractedImage().copyTo(buffer[file_idx]);

      //std::stringstream filename;
      //filename << complete_file_root << std::setw(5) << std::setfill('0') << file_idx << ".tif";
      //cv::imwrite(filename.str(), buf->GetBackgroundSubtractedImage());

      file_idx++;

      if (file_idx == buffer.size())
      {
         active = false;
         emit ActiveStateChanged(active);
         WriteBuffer();
      }
   }

}

void ImageWriter::WriteBuffer()
{
   for (int i = 0; i < file_idx; i++)
   {
      std::stringstream filename;
      filename << complete_file_root << std::setw(5) << std::setfill('0') << i << ".tif";
      cv::imwrite(filename.str(), buffer[i]);
   }
}
