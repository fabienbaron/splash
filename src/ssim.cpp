#include "core.h"
#include "similarityMetric.h"
#include "ssim.h"

calcSSIM :: calcSSIM()
{
  K1 = 0.01;
  K2 = 0.03;
  gaussian_window = 11;
  gaussian_sigma = 1.5;
  L = 255;
  ssim_map = NULL;
  for (int i=0; i < 4; i++)
    mssim_value.val[i] = -1; // Initialize with an out of bound value of mssim [0,1]
  cs_map = NULL;
  for (int i=0; i < 4; i++)
    mean_cs_value.val[i] = -1; // Initialize with an out of bound value of mssim [0,1]
}

void calcSSIM :: releaseSSIM_map() { 
  if (ssim_map != NULL)
    cvReleaseImage(&ssim_map);
  ssim_map = NULL;
}

void calcSSIM :: releaseCS_map() { 
  if (cs_map != NULL)
    cvReleaseImage(&cs_map); 
  cs_map = NULL;
}

int calcSSIM :: print_map()
{
  if (ssim_map == NULL)
  {
    cout<<"Error>> No Index_map_created.\n";
    return 0;
  }
  cvSave("imgSSIM.xml", ssim_map, NULL, "TESTing Index map");
  return 1;
}

Scalar calcSSIM :: compare(const Mat& mat1, const Mat& mat2)
{
  // default settings
  const double C1 = (K1 * L) * (K1 * L); 
  const double C2 = (K2 * L) * (K2 * L);
  
  Mat img1_sq, img2_sq, img1_img2;
  //Squaring the images thus created
  pow(img1, 2, img1_sq);
  pow(img2, 2, img2_sq);
  multiply(img1, img2, img1_img2, 1);

  Mat mu1, mu2, mu1_sq, mu2_sq, mu1_mu2, sigma1_sq, sigma2_sq, sigma12;

  //PRELIMINARY COMPUTING
  //gaussian smoothing is performed
  cvSmooth(img1, mu1, CV_GAUSSIAN, gaussian_window, gaussian_window, gaussian_sigma);
  cvSmooth(img2, mu2, CV_GAUSSIAN, gaussian_window, gaussian_window, gaussian_sigma);

  //gettting mu, mu_sq, mu1_mu2
  pow(mu1, 2, mu1_sq);
  pow(mu2, 2, mu2_sq);
  multiply(mu1, mu2, mu1_mu2, 1);

  //calculating sigma1, sigma2, sigma12
  cvSmooth(img1_sq, sigma1_sq, CV_GAUSSIAN, gaussian_window, gaussian_window, gaussian_sigma);
  cvSub(sigma1_sq, mu1_sq, sigma1_sq);

  cvSmooth(img2_sq, sigma2_sq, CV_GAUSSIAN, gaussian_window, gaussian_window, gaussian_sigma);
  cvSub(sigma2_sq, mu2_sq, sigma2_sq);

  cvSmooth(img1_img2, sigma12, CV_GAUSSIAN, gaussian_window, gaussian_window, gaussian_sigma);
  cvSub(sigma12, mu1_mu2, sigma12);
  
  Mat numerator1, numerator2, numerator, denominator1, denominator2, denominator;

  // FORMULA to calculate SSIM
  // (2*mu1_mu2 + C1)
  cvScale(mu1_mu2, numerator1, 2);
  cvAddS(numerator1, cvScalarAll(C1), numerator1);
  // (2*sigma12 + C2) 
  cvScale(sigma12, numerator2, 2);
  cvAddS(numerator2, cvScalarAll(C2), numerator2);
  // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
  cvMul(numerator1, numerator2, numerator, 1);

  // (mu1_sq + mu2_sq + C1)
  cvAdd(mu1_sq, mu2_sq, denominator1);
  cvAddS(denominator1, cvScalarAll(C1), denominator1);
  // (sigma1_sq + sigma2_sq + C2) >>>
  cvAdd(sigma1_sq, sigma2_sq, denominator2);
  cvAddS(denominator2, cvScalarAll(C2),denominator2);
  // ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
  cvMul(denominator1, denominator2, denominator, 1);

  //ssim map and cs_map
  Mat ssim_map, cs_map;
  // SSIM_INDEX map 
  // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
  cvDiv(numerator, denominator, ssim_map, 1);
  // Contrast Structure CS_index map
  // (2*sigma12 + C2)./(sigma1_sq + sigma2_sq + C2)
  cvDiv(numerator2, denominator2, cs_map, 1);

  // average is taken for both SSIM_map and CS_map 
  mssim_value = cvAvg(ssim_map);
  mean_cs_value = cvAvg(cs_map);

  return mssim_value;
}


