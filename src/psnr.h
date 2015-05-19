#ifndef _psnr_h
#define _psnr_h

// PSNR class
// inheriting from SimilarityMetric class
class calcPSNR : public SimilarityMetric
{

private:
  int L;
  Scalar mse;
  Scalar PSNR;

public:

  calcPSNR();

  // get and set functions
  void setL(int value)
  {
    L = value;
  }
  void setMSE(CvScalar value)
  {
    mse = value;
  }
  Scalar getPSNR()
  {
    return PSNR;
  }

  // calculates PSNR using OpenCV functions
  virtual float compare(const Mat &source1, const Mat &source2);

};

#endif
