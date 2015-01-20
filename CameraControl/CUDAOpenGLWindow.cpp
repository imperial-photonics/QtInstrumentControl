#include "OpenGLWindow.h"

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include <cassert>

void CUDAOpenGLWindow::Initialize()
{
   OpenGLWindow::Initialize();
   AllocateBuffer();
}

bool CUDAOpenGLWindow::DrawImage(unsigned char * pCudaImageMem, int width, int height, int pitch, int n_bytes)
{
   context->makeCurrent(this);
   
   QSize sz = buffer_size;
   assert( sz.width() == width );
   assert( sz.height() == height );
   assert( this->n_bytes == n_bytes );
       
   ///// STEP 1, copy the data from CUDA memory into a GL buffer
	if(pCudaImageMem != NULL)	// if a null pointer is passed, just draw whatever is in the buffer.
	{
	   // Map the GL buffer to cuda memory space
		void * GLtemp;
		cudaError_t c_err;
		c_err = cudaGLMapBufferObject(&GLtemp, bufferID);
		if(c_err != cudaSuccess) throw;

		// Copy the data from CUDA's memory to the buffer memory
		// Note: This copy is not the most ideal for performance, but keeps our example more simple
		//       In an optimized configuration one would allocate GL buffers and then map them to CUDA as the output memory
		//       for any image processing, then unmap them.  We make a copy instead, which completely isolates the CUDA example from
		//       OpenGL code.  
		c_err = cudaMemcpy2D(GLtemp,                     // the destination
		     					   width * n_bytes,            // width of each row of data to be copies, in bytes
								   (void*)pCudaImageMem,       // source of the copy
								   pitch,                      // source data pitch
								   width * n_bytes,            // width of each row of data to be copied, in bytes
								   height,                     // number of rows to copy
								   cudaMemcpyDeviceToDevice);  // the copy is from one GPU memory location to another

		if(c_err != cudaSuccess) throw;
		cudaGLUnmapBufferObject(bufferID);      // unmap the buffer object so GL has it back
   }
       
	///// STEP 2, Create an OpenGL texture from the data stored in the OpenGL buffer
	if(!DrawBufferStep())
      throw;

   //// STEP 3, We'll draw a quad on the screen, with the texture we just created.
   // This code is in the DrawTexture function and is also called whenever we need to repaint the window
   if(!DrawTextureStep()) 
      throw;
       
   // Everything's OK.  return
   return true;
      
}

// This funciton implements only the 2nd & 3rd steps of drawing an image.  It assumes the buffer has been writted directly
bool CUDAOpenGLWindow::DrawBuffer()
{
   context->makeCurrent(this);

   if(!glIsBuffer(bufferID))
      throw;

   if(!glIsTexture(textureID)) 
      throw;   

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

   if(!DrawBufferStep()) 
      throw;
   
   if(!DrawTextureStep())
      throw;
       
   return true;
}

bool CUDAOpenGLWindow::DrawBufferStep()
{
	// Select our Image Texture
	glBindTexture( GL_TEXTURE_2D, textureID); 

	// And Bind the OpenGL Buffer for the pixel data
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER, bufferID);    
	    
   GLenum format, type;
   if (n_bytes == 1)
   {
      format = GL_LUMINANCE;
      type = GL_UNSIGNED_BYTE;
   }
   else if (n_bytes == 2)
   {
      format = GL_RG;
      type = GL_UNSIGNED_BYTE;
   }

   // the glTexSubImage command copies the data from the bound GL buffer into the texture
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, buffer_size.width(), buffer_size.height(), format, GL_UNSIGNED_BYTE, 0); 
	   
	// done with the GBuffer
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, NULL);

	// A quick error check
	GLenum err = glGetError();
   if(err != GL_NO_ERROR) 
      throw;
 
   return true;
}

bool CUDAOpenGLWindow::DrawTextureStep()
{
   if(!glIsTexture(textureID)) 
      return false;

    // bind the texture
    glBindTexture( GL_TEXTURE_2D, textureID);

    // Set the texture interopolation parameters
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // Now draw a quad.  We won't attempt any scaling in this simple example.  If the window is a different aspect ratio then the image, it will be distorted
              
    glBegin(GL_QUADS);
      glTexCoord2f( 0, 0);
      glVertex3f(0,0,0);

      glTexCoord2f(0,1.0f);
      glVertex3f(0,1.0f,0);

      glTexCoord2f(1.0f,1.0f);
      glVertex3f(1.0f,1.0f,0);
            
      glTexCoord2f(1.0f,0);
      glVertex3f(1.0f,0,0);
   glEnd();

   // Done with the texture 
   glBindTexture(GL_TEXTURE_2D ,0);

   // Finally swap the back framebuffer and the front framebuffer actually creating the draw
   context->swapBuffers(this);

   if(glGetError() != GL_NO_ERROR) 
      throw;

   return true;
}





// This fuction allocates the GLbuffer & GLtexture needed to draw an image
// If a buffer already exists, it is first freed
// width & height are the size of the image in pixels, color indicates if the 
// image is RGBA (8-bit/channel) or 16-bit Grayscale
bool CUDAOpenGLWindow::AllocateBuffer()
{
   context->makeCurrent(this);

   // If the buffers exist, delete them.
	if(glIsBuffer(bufferID))
	   glDeleteBuffers(1,&bufferID);
	if(glIsTexture(textureID))
	   glDeleteTextures(1,&textureID);
       
   // Create a texture for drawing the image
   glGenTextures(1,&textureID);
   glBindTexture( GL_TEXTURE_2D, textureID);  
       
   // glTexImage2D actually allocates the texture data according to the desired format.  
   // Since we are not filling the texture right now, the data pointer parameter is null
   GLuint tex_format = (n_bytes == 2) ? GL_RG : GL_LUMINANCE;
	
   //	tex_format = GL_RGBA;
   glTexImage2D( GL_TEXTURE_2D, 0, tex_format, buffer_size.width(), buffer_size.height(), 0, tex_format, GL_UNSIGNED_BYTE, NULL);
        
   // Quick error check
   if(glGetError() != GL_NO_ERROR) 
      throw;

   // Now we need to generate an OpenGL buffer object we can share with CUDA
   glGenBuffers(1,&bufferID);
   glBindBuffer( GL_PIXEL_UNPACK_BUFFER, bufferID); 	
	  
   // Now actually allocate the data
   glBufferData( GL_PIXEL_UNPACK_BUFFER, buffer_size.width() * buffer_size.height() * n_bytes, NULL, GL_DYNAMIC_DRAW);       
         
   // Quick error check
   GLint err = glGetError();
   if(err != GL_NO_ERROR) 
      throw;

   // Now register the buffer so CUDA can use it
   cudaError_t c_err = cudaGLRegisterBufferObject(bufferID);
   if(c_err != cudaSuccess) 
      throw;

   if(!glIsBuffer(bufferID))
      throw;

   // Everything is OK!
   return true;
}


void * CUDAOpenGLWindow::GetCPUBufferPtr()
{
	if(!glIsBuffer(bufferID)) return NULL;
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,bufferID);
	void * retptr =  glMapBuffer(GL_PIXEL_UNPACK_BUFFER,GL_READ_WRITE);
	GLenum err = glGetError();
	if(err == GL_NO_ERROR) return retptr;
	return NULL;
}

void CUDAOpenGLWindow::ReleaseCPUBufferPtr()
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,bufferID);
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,NULL);	
}


void * CUDAOpenGLWindow::GetCudaBufferPtr()
{
	void * retptr;
	cudaError_t err = cudaGLMapBufferObject(&retptr,bufferID);
	if(err != cudaSuccess) return NULL;
	return retptr;
}

void CUDAOpenGLWindow::ReleaseCudaBufferPtr()
{
	cudaGLUnmapBufferObject(bufferID);	
}


