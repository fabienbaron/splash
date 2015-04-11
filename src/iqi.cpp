#include "core.h"
#include "similarityMetric.h"
#include "iqi.h"

calcQualityIndex :: calcQualityIndex()
{
  B = 8;
  for (int i=0; i < 4; i++)
    image_quality_value.val[i] = -2;   // Initialize with an out of bound value for image_quality_value [-1.1]
}

calcQualityIndex :: ~calcQualityIndex()
{
  image_quality_map.release();
}

void calcQualityIndex :: releaseImageQuality_map() {
  image_quality_map.release();
}

float calcQualityIndex :: compare(Mat& img1, Mat& img2)
{
  //Image squares
  Mat img1_sq, img2_sq, img1_img2;
  
  //Squaring the images thus created
  pow(img1, 2, img1_sq);
  pow(img2, 2, img2_sq);
  img1_img2 = img1.mul(img2);

  Mat mu1, mu2, mu1_sq, mu2_sq, mu1_mu2, sigma1_sq, sigma2_sq, sigma12;

  //PRELIMINARY COMPUTING
  
  //average smoothing is performed
  Size blur_kernel = Size(B, B);
  blur(img1, mu1, blur_kernel);
  blur(img2, mu2, blur_kernel);

  //gettting mu, mu_sq, mu1_mu2
  pow(mu1, 2, mu1_sq);
  pow(mu2, 2, mu2_sq);
  mu1_mu2 = mu1.mul(mu2);

  //calculating sigma1, sigma2, sigma12
  blur(img1_sq, sigma1_sq, blur_kernel);
  sigma1_sq -= mu1_sq;

  blur(img2_sq, sigma2_sq, blur_kernel);
  sigma2_sq -= mu2_sq;

  blur(img1_img2, sigma12, blur_kernel);
  sigma12 -= mu1_mu2;

  // creating buffers for numerator and denominator
  Mat numerator1, numerator, denominator1, denominator2, denominator;

  // FORMULA to calculate Image Quality Index

  // (4*sigma12)
  numerator1 = 4 * sigma12;

  // (4*sigma12).*(mu1*mu2)
  numerator = numerator1.mul(mu1_mu2);

  // (mu1_sq + mu2_sq)
  denominator1 = mu1_sq + mu2_sq;

  // (sigma1_sq + sigma2_sq)
  denominator2 = sigma1_sq + sigma2_sq;

  // ((mu1_sq + mu2_sq).*(sigma1_sq + sigma2_sq))
  denominator = denominator1.mul(denominator2);

  // cvDiv doesnt work
  // FB: because of zeroes in denominator... 
  // original code replaces aberrant quotient values by 1.
  image_quality_map = numerator / denominator;

  // image_quality_map created in image_quality_map
  // average is taken 
  image_quality_value = mean(image_quality_map);

  return image_quality_value.val[0];
}


