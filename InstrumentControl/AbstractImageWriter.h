#pragma once

#include <QFileDialog>
#include "ThreadedObject.h"
#include <QThread>
#include <string>

class AbstractImageWriter : public ThreadedObject
{
   Q_OBJECT

public:
   AbstractImageWriter(QObject* parent = 0, QThread* thread = 0);


   void SetFilenameRoot(const QString& file_root_);
   void SetFolder(const QString& folder_);

   void ChooseFolder();

   const QString& GetFilenameRoot() { return file_root; }
   const QString& GetFolder() { return folder; }

   void SetBufferSize(int buffer_size_);
   int GetBufferSize() { return buffer_size; }

   void SetActive(bool active_);

   /*
      Should save a single image
   */
   virtual void SaveSingle() = 0;


   /*
      This function will get called when a new image is available
   */
   virtual void ImageUpdated() = 0;

signals:
   void FolderChanged(const QString& folder);
   void FilenameRootChanged(const QString& filename_root);
   void BufferSizeChanged(int buffer_size);
   void ActiveStateChanged(bool active);

protected:

   virtual void WriteBuffer() = 0;
   virtual void InitBuffer() = 0;

private:

   bool active;
   int file_idx;
   std::string complete_file_root;
   QString file_root;
   QString folder;

   int buffer_size;
};
