#include "core.h"
#include "similarityMetric.h"
#include "psnr.h"

calcPSNR :: calcPSNR()
{
  L = 255;
  for (int i=0; i < 4; i++)
    mse.val[i] = 1;
  for (int i=0; i < 4; i++)
    PSNR.val[i] = 0;
}

float calcPSNR :: compare(const Mat& img1, const Mat& img2)
{
  //creating diff and difference squares
  Mat diff, diff_sq;
  absdiff(img1, img2, diff);
  //Squaring the images thus created
  pow(diff, 2, diff_sq);
  mse = mean(diff_sq);
  return -10.0*log10((L*L)/mse.val[0]);;
}


