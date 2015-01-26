#include "AbstractImageWriter.h"

AbstractImageWriter::AbstractImageWriter(QObject* parent, QThread* thread) :
ThreadedObject(parent, thread)
{
   file_root = "image ";
   active = false;
 
   StartThread();
};

void AbstractImageWriter::SetFilenameRoot(const QString& file_root_)
{
   file_root = file_root_;
   QString cf = folder;
   complete_file_root = cf.append("/").append(file_root).toStdString();
   emit FilenameRootChanged(file_root_);
}

void AbstractImageWriter::ChooseFolder()
{
   folder = QFileDialog::getExistingDirectory(0, tr("Choose folder"), "R:/");
   QString cf = folder;
   complete_file_root = cf.append("/").append(file_root).toStdString();
   emit FolderChanged(folder);
}

void AbstractImageWriter::SetFolder(const QString& folder_)
{
   folder = folder_;
   emit FolderChanged(folder);
}

void AbstractImageWriter::SetBufferSize(int buffer_size_)
{
   if (!active && buffer_size > 0)
   {
      buffer_size = buffer_size_;
      InitBuffer();

      emit BufferSizeChanged(buffer_size);
   }

}

void AbstractImageWriter::SetActive(bool active_)
{
   // If we're stopping early
   if (active & !active_)
   {
      active = false;
      WriteBuffer();
   }

   if (active_ && folder.isEmpty())
      GetFolder();

   active = active_;
   file_idx = 0;

   emit ActiveStateChanged(active);
}

