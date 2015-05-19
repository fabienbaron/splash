#include "core.h"
#include "similarityMetric.h"
#include "ssim.h"

calcSSIM :: calcSSIM()
{
  K1 = 1e-9;
  K2 = 3e-9;
  gaussian_window = 9;
  gaussian_sigma = 1.5;
  L = 1.;
}

//int calcSSIM :: print_map()
//{
//  if (ssim_map == NULL)
//  {
//    cout<<"Error>> No Index_map_created.\n";
//    return 0;
//  }
//  cvSave("imgSSIM.xml", ssim_map, NULL, "TESTing Index map");
//  return 1;
//}

void calcSSIM :: releaseSSIM_map()
{
  ssim_map.release();
}

void calcSSIM :: releaseCS_map()
{
  cs_map.release();
}

float calcSSIM :: compare(const Mat &img1, const Mat &img2)
{
  // default settings
  const double C1 = (K1 * L) * (K1 * L);
  const double C2 = (K2 * L) * (K2 * L);


  Size gaussian_kernel;
  gaussian_kernel.width = gaussian_window;
  gaussian_kernel.height = gaussian_window;

  Mat img1_sq, img2_sq, img1_img2;
  //Squaring the images thus created
  pow(img1, 2, img1_sq);
  pow(img2, 2, img2_sq);
  multiply(img1, img2, img1_img2, 1);

  Mat mu1, mu2, mu1_sq, mu2_sq, mu1_mu2, sigma1_sq, sigma2_sq, sigma12;

  //PRELIMINARY COMPUTING
  //gaussian smoothing is performed
  GaussianBlur(img1, mu1, gaussian_kernel, gaussian_sigma);
  GaussianBlur(img2, mu2, gaussian_kernel, gaussian_sigma);

  //gettting mu, mu_sq, mu1_mu2
  pow(mu1, 2, mu1_sq);
  pow(mu2, 2, mu2_sq);
  multiply(mu1, mu2, mu1_mu2, 1);

  //calculating sigma1, sigma2, sigma12
  GaussianBlur(img1_sq, sigma1_sq, gaussian_kernel, gaussian_sigma);
  sigma1_sq -= mu1_sq;

  GaussianBlur(img2_sq, sigma2_sq, gaussian_kernel, gaussian_sigma);
  sigma2_sq -= mu2_sq;

  GaussianBlur(img1_img2, sigma12, gaussian_kernel, gaussian_sigma);
  sigma12 -= mu1_mu2;

  Mat numerator1, numerator2, numerator, denominator1, denominator2, denominator;

  // FORMULA to calculate SSIM
  // (2*mu1_mu2 + C1)
  numerator1 = 2 * mu1_mu2 + C1;
  // (2*sigma12 + C2)
  numerator2 = 2 * sigma12 + C2;
  // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
  numerator =  numerator1.mul(numerator2);

  // (mu1_sq + mu2_sq + C1)
  denominator1 = mu1_sq + mu2_sq + C1;

  // (sigma1_sq + sigma2_sq + C2)
  denominator2 = sigma1_sq + sigma2_sq + C2;

  // ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
  denominator = denominator1.mul(denominator2);

  //ssim map and cs_map
  Mat ssim_map, cs_map;
  // SSIM_INDEX map
  // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
  ssim_map = numerator / denominator;
  // Contrast Structure CS_index map
  // (2*sigma12 + C2)./(sigma1_sq + sigma2_sq + C2)
  cs_map = numerator2 / denominator2;

  // average is taken for both SSIM_map and CS_map
  ssim_value = mean(ssim_map);
  mean_cs_value = mean(cs_map);

  return 1. / ssim_value.val[0] - 1;
}


