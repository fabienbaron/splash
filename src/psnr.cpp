#include "core.h"
#include "similarityMetric.h"
#include "psnr.h"

calcPSNR :: calcPSNR()
{
  L = 255;
  for (int i=0; i < 4; i++)
    mse.val[i] = -1;
  for (int i=0; i < 4; i++)
    PSNR.val[i] = -1;
}

Scalar calcPSNR :: compare(Mat& img1, Mat& img2)
{
  //creating diff and difference squares
  Mat diff, diff_sq;
    
  absdiff(img1, img2, diff);
  //Squaring the images thus created
  pow(diff, 2, diff_sq);
  mse = mean(diff_sq);
  
  PSNR.val[0] = 10.0*log10((L*L)/mse.val[0]);
  PSNR.val[1] = 10.0*log10((L*L)/mse.val[1]);
  PSNR.val[2] = 10.0*log10((L*L)/mse.val[2]);
  PSNR.val[3] = 10.0*log10((L*L)/mse.val[3]);
  return PSNR;
}


