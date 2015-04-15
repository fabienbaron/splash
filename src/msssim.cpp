#include "core.h"
#include "similarityMetric.h"
#include "ssim.h"
#include "msssim.h"

calcMSSSIM :: calcMSSSIM()
{
  K1 = 0.001;
  K2 = 0.003;
  gaussian_window = 5;
  gaussian_sigma = 1.5;
  level = 5;
  L = 1;
  ms_ssim_map = NULL;
  for (int i=0; i < 4; i++)
    ms_ssim_value.val[i] = -1; // Initialize with an out of bound value of mssim [0,1]

  // setting alpha, beta, gamma default values
  float alpha_t[5] = {0.0, 0.0, 0.0, 0.0, 0.1333};
  float beta_t[5] = {0.0448, 0.2856, 0.3001, 0.2363, 0.1333};
  float gamma_t[5] = {0.0448, 0.2856, 0.3001, 0.2363, 0.1333};
  alpha = new float[level];
  beta = new float[level];
  gamma = new float[level];
  memcpy(alpha, alpha_t, sizeof(alpha_t));
  memcpy(beta, beta_t, sizeof(beta_t));
  memcpy(gamma, gamma_t, sizeof(gamma_t));
}

calcMSSSIM :: ~calcMSSSIM()
{
  delete[] alpha;
  delete[] beta;
  delete[] gamma;
  int i;
  for (i=0; i < level; i++) 
       ms_ssim_map[i].release();
  
  if (ms_ssim_map !=NULL) 
    delete ms_ssim_map;
}

void calcMSSSIM :: releaseMSSSIM_map() {
  int i;
  for (i=0; i < level; i++) 
       ms_ssim_map[i].release();
  
  if (ms_ssim_map != NULL)
    delete ms_ssim_map;
}

// Prints all index maps of all the levels into different xml files
//int calcMSSSIM :: print_map()
//{
//  if (ms_ssim_map == NULL)
//  {
//    cout<<"Error>> No Index_map_created.\n";
//    return 0;
//  }
//  char file_name[50];
//  // Printing the MS-SSIM_Map
//  for (int i=0; i < level; i++)
//  {
//    sprintf(file_name, "img_MS-SSIM_map_level_%d.xml", i);
//    cvSave(file_name, ms_ssim_map[i], NULL, "Testing MS-SSIM Index map");
//  }
//  return 1;
//}

float calcMSSSIM :: compare(const Mat& source1, const Mat& source2)
{
  // image dimensions
  int x = source1.cols, y = source1.rows;
  // int nChan = source1.nChannels;
  // int d = source1.depth;

  // creating a object of class calcSSIM 
  // setting the initial parameters values
  calcSSIM ssim;
  ssim.setK1(K1);
  ssim.setK2(K2);
  ssim.setGaussian_window(gaussian_window);
  ssim.setGaussian_sigma(gaussian_sigma);
  ssim.setL(L);
  //Creating an array of Mats for ssim_map at various levels
  // ms_ssim_map = new Mat[level];

  Mat downsampleSrc1, downsampleSrc2, temp;
  Size downs_size;
  Scalar mssim_t, mcs_t;
  for (int i=0; i<level; i++)
  {
    //Downsampling of the original images
   
    //Downsampling the images
    downs_size = Size((int)(x/pow(2, i)), (int)(y/pow(2, i)));

    downsampleSrc1 = cv::Mat(downs_size, CV_32FC1);
    downsampleSrc2 = cv::Mat(downs_size, CV_32FC1);
    resize(source1, downsampleSrc1, downs_size, 0, 0, INTER_NEAREST); // note: maybe change interpolation function
    resize(source2, downsampleSrc2, downs_size, 0, 0, INTER_NEAREST);
    ssim.compare(downsampleSrc1, downsampleSrc2);
    mssim_t = ssim.getMSSIM();
    mcs_t = ssim.getMeanCSvalue();
    
    //releasing the CS_map since not required anymore
    ssim.releaseCS_map(); 
    
    // calculating the withed average to find ms-ssim
    for (int j=0; j < 4; j++)
    {
      if (i == 0)
        ms_ssim_value.val[j] = pow((mcs_t.val[j]), (double)(beta[i]));
      else 
        if (i == level-1)
          ms_ssim_value.val[j] = (ms_ssim_value.val[j]) * pow((mssim_t.val[j]), (double)(beta[i]));
        else
          ms_ssim_value.val[j] = (ms_ssim_value.val[j]) * pow((mcs_t.val[j]), (double)(beta[i]));
    }
    //Release images
    downsampleSrc1.release();
    downsampleSrc2.release();

  }
  return 1./ms_ssim_value.val[0]-1.;
}



