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

float calcQualityIndex :: compare(const Mat& img1, const Mat& img2)
{
  //Image squares
  Mat img1_sq, img2_sq, img12;
  
  //Squaring the images thus created
  pow(img1, 2, img1_sq);
  pow(img2, 2, img2_sq);
  img12 = img1.mul(img2);

  Mat img1_sum, img2_sum, img12_sum, img1_sq_sum, img2_sq_sum, img12_sum_mul, img12_sq_sum_mul, numerator, denominator;

  //PRELIMINARY COMPUTING
  
  //average smoothing is performed
  Size blur_kernel = Size(B, B);
  blur(img1, img1_sum, blur_kernel);
  blur(img2, img2_sum, blur_kernel);
  blur(img1_sq, img1_sq_sum, blur_kernel);
  blur(img2_sq, img2_sq_sum, blur_kernel);
  blur(img12  , img12_sum, blur_kernel);
  img12_sum_mul = img1_sum.mul(img2_sum);
  img12_sq_sum_mul = img1_sum.mul(img1_sum) + img2_sum.mul(img2_sum);

  numerator = 4*(B*B*img12_sum - img12_sum_mul);
  numerator = numerator.mul(img12_sum_mul);

  denominator = B*B*(img1_sq_sum + img2_sq_sum) - img12_sq_sum_mul;
  denominator = denominator.mul(img12_sq_sum_mul);

  image_quality_map = Mat::ones(numerator.cols, numerator.rows,CV_32FC1 );


  for(int i=0;i<numerator.cols;i++)
    {
      for(int j=0;j<numerator.rows;j++)
	{
	  if( fabs(denominator.at<float>(i,j)) < 1e-10)
	    {
	      if (fabs(img12_sq_sum_mul.at<float>(i,j)) > 1e-10)
		image_quality_map.at<float>(i,j)= 2.*img12_sum_mul.at<float>(i,j)/img12_sq_sum_mul.at<float>(i,j);
	    }
	  else 	 
	    image_quality_map.at<float>(i,j) = numerator.at<float>(i,j)/denominator.at<float>(i,j);
	}
    }

  // image_quality_map created in image_quality_map
  // average is taken 
  image_quality_value = mean(image_quality_map);

  return 0.5*(image_quality_value.val[0]+1.);
}


