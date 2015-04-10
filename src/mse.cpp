#include "core.h"
#include "similarityMetric.h"
#include "mse.h"    

calcMSE :: calcMSE()
{
  for (int i=0; i < 4; i++)
    mse.val[i] = -1;
}

Scalar calcMSE :: compare(const Mat& mat1, const Mat& mat2)
{
  //creating diff and difference squares
  Mat diff; 
  Mat diff_sq;

  absdiff(mat1, mat2, diff);
  pow(diff, 2, diff_sq);
  Scalar num=sum(diff_sq);
  
  pow(mat1, 2, diff_sq); // re-using diff_sq
  Scalar denom = sum(diff_sq);

  return num/denom;
}

