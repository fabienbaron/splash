#ifndef _ssim_h
#define _ssim_h

// SSIM class
// inheriting SimilarityMetric
class calcSSIM : public SimilarityMetric
{

private:

  // parameters
  double K1;
  double K2;
  int gaussian_window;
  double gaussian_sigma;
  int L;
  Mat ssim_map;
  Scalar ssim_value;
  Mat cs_map;
  Scalar mean_cs_value; // mean of contrast, structure (part of l,c,s)

public:

  calcSSIM();

  void setK1(double val)
  {
    K1 = val;
  }
  void setK2(double val)
  {
    K2 = val;
  }
  void setGaussian_window(int val)
  {
    gaussian_window = val;
  }
  void setGaussian_sigma(int val)
  {
    gaussian_sigma = val;
  }
  void setL(int val)
  {
    L = val;
  }

  Scalar getMSSIM()
  {
    return ssim_value;
  }
  Scalar getMeanCSvalue()
  {
    return mean_cs_value;
  }
  Mat &getSSIM_map()
  {
    return ssim_map;
  }
  Mat &getCS_map()
  {
    return cs_map;
  }

  // release SSIM_map
  void releaseSSIM_map();

  // release CS_map
  void releaseCS_map();

  // print_map
  int print_map();

  // calculating mean SSIM value using openCV functions
  virtual float compare(const Mat &img1, const Mat &img2);

};

#endif


