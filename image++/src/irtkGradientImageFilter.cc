/*=========================================================================

  Library   : Image Registration Toolkit (IRTK)
  Module    : $Id$
  Copyright : Imperial College, Department of Computing
              Visual Information Processing (VIP), 2009 onwards
  Date      : $Date$
  Version   : $Revision$
  Changes   : $Author$

=========================================================================*/

#include <irtkImage.h>

#include <irtkGradientImageFilter.h>

template <class VoxelType> irtkGradientImageFilter<VoxelType>::irtkGradientImageFilter(int type)
{
  _type = type;
}

template <class VoxelType> Bool irtkGradientImageFilter<VoxelType>::RequiresBuffering(void)
{
  return True;
}

template <class VoxelType> const char *irtkGradientImageFilter<VoxelType>::NameOfClass()
{
  return "irtkGradientImageFilter";
}

template <class VoxelType> void irtkGradientImageFilter<VoxelType>::Initialize()
{
  // Print debugging information
  this->Debug("irtkImageToImage::Initialize");

  // Check inputs and outputs
  if (this->_input == NULL) {
    cerr << this->NameOfClass() << "::Run: Filter has no input" << endl;
    exit(1);
  }

  if (this->_output == NULL) {
    cerr << this->NameOfClass() << "::Run: Filter has no output" << endl;
    exit(1);
  }

  if (this->_input->IsEmpty() == True) {
    cerr << this->NameOfClass() << "::Run: Input is empty" << endl;
    exit(1);
  }

  if (this->_input->GetT() > 1) {
    cerr << this->NameOfClass() << "::Run: Only implemented for images with t = 1" << endl;
    exit(1);
  }

  // Check whether filter requires buffering
  if (this->RequiresBuffering()) {
    this->Debug("irtkGradientImageFilter::Initialize: Filter requires buffering");

    // Check whether filter has external buffer
    if (this->_input == this->_output) {
      this->Debug("irtkGradientImageFilter::Initialize: Filter has internal buffer");
      this->_tmp    = this->_output;
      this->_output = new irtkGenericImage<VoxelType>;
    } else {
      this->Debug("irtkGradientImageFilter::Initialize: Filter has external buffer");
      this->_tmp    = NULL;
    }
  } else {
    this->Debug("irtkGradientImageFilter::Initialize: Filter requires no buffering");
  }

  // Make sure that output has the correct dimensions
  if (_type == GRADIENT_VECTOR) {
  	irtkImageAttributes attr = this->_input->GetImageAttributes();
    attr._t = 3;
    this->_output->Initialize(attr);
  } else {
    if (this->_input != this->_output) this->_output->Initialize(this->_input->GetImageAttributes());
  }
}

template <class VoxelType> void irtkGradientImageFilter<VoxelType>::Run()
{
  double dx, dy, dz;
  int x, y, z, x1, y1, z1, x2, y2, z2;

  // Do the initial set up
  this->Initialize();

  for (z = 0; z < this->_input->GetZ(); ++z) {
    for (y = 0; y < this->_input->GetY(); ++y) {
      for (x = 0; x < this->_input->GetX(); ++x) {

        x1 = x - 1;
        if (x1 < 0) x1 = 0;
        x2 = x + 1;
        if (x2 > this->_input->GetX()-1) x2 = this->_input->GetX()-1;
        dx = (this->_input->Get(x2, y, z) - this->_input->Get(x1, y, z)) / ((x2 - x1)*this->_input->GetXSize());

        y1 = y - 1;
        if (y1 < 0) y1 = 0;
        y2 = y + 1;
        if (y2 > this->_input->GetY()-1) y2 = this->_input->GetY()-1;
        dy = (this->_input->Get(x, y2, z) - this->_input->Get(x, y1, z)) / ((y2 - y1)*this->_input->GetYSize());

        z1 = z - 1;
        if (z1 < 0) z1 = 0;
        z2 = z + 1;
        if (z2 > this->_input->GetZ()-1) z2 = this->_input->GetZ()-1;
        dz = (this->_input->Get(x, y, z2) - this->_input->Get(x, y, z1)) / ((z2 - z1)*this->_input->GetZSize());

        switch (_type) {
          case GRADIENT_X:
            this->_output->PutAsDouble(x, y, z, 0, dx);
            break;
          case GRADIENT_Y:
            this->_output->PutAsDouble(x, y, z, 0, dy);
            break;
          case GRADIENT_Z:
            this->_output->PutAsDouble(x, y, z, 0, dz);
            break;
          case GRADIENT_MAGNITUDE:
            this->_output->PutAsDouble(x, y, z, 0, sqrt(dx*dx + dy*dy + dz*dz));
            break;
          case GRADIENT_VECTOR:
            this->_output->PutAsDouble(x, y, z, 0, dx);
            this->_output->PutAsDouble(x, y, z, 1, dy);
            this->_output->PutAsDouble(x, y, z, 2, dz);
            break;
          default:
            cerr << this->NameOfClass() << "::Run: Unknown gradient computation" << endl;
            exit(1);
        }
      }
    }
  }

  // Do the final cleaning up
  this->Finalize();
}

template class  irtkGradientImageFilter<unsigned char>;
template class  irtkGradientImageFilter<short>;
template class  irtkGradientImageFilter<float>;