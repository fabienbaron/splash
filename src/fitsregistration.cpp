//    SPLASH - FITS image registration
//    Copyright (C) 2015       Fabien Baron
//    Copyright (C) 2012-2015  Dushyant Goyal
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Including core.h
#include "core.h"


#define MAX_SOURCE_SIZE (0x100000)
using namespace cv;
//using namespace std;


// Including the Similarity Metric Header files
#include "similarityMetric.h"
#include "mse.h"
#include "psnr.h"
#include "ssim.h"
#include "msssim.h"
#include "iqi.h"

// FITS library
#include "fitsio.h"

#define ALGO_MSE   0
#define ALGO_PSNR  1
#define ALGO_SSIM  2
#define ALGO_MSSSIM 3
#define ALGO_IQI   4

const char* algo_names[] = {"MSE", "PSNR","SSIM","MS-SSIM", "IQI"};


Mat registration(const Mat& img1, const Mat& img2, float& metric_value, float& dx, float& dy, const int range, const float subrange, const int algo, SimilarityMetric* method);

void writeMatToFile(const Mat& m, const char* filename)
{
    ofstream fout(filename);

    if(!fout)
    {
        cout<<"File Not Opened"<<endl;  return;
    }

    for(int i=0; i<m.rows; i++)
    {
        for(int j=0; j<m.cols; j++)
        {
            fout<<m.at<float>(i,j)<<"\t";
        }
        fout<<endl;
    }

    fout.close();
}


void printerror(int status)
{
	if (status)
        {
		fits_report_error(stderr, status); /* print error report */
		exit(status); /* terminate the program, returning error status */
	    }
	return;
}


Mat cvFITS(const char *filename)
{
    // A lot of ugly steps here...

    fitsfile *fptr; /* pointer to FITS files, defined in fitsio.h */
    int status=0, dummy_int, nfound;
    char dummy_char[50];
    long nullval_lng = 0;
    double nullval = 0.;
    if (fits_open_file(&fptr, filename, READONLY, &status))
      printerror(status);

    fits_read_key_lng(fptr, "NAXIS", &nullval_lng, dummy_char, &status);
    long* in_naxes = new long[2];

    if (fits_read_keys_lng(fptr, "NAXIS", 1, 2, in_naxes, &nfound, &status))
      printerror(status);

     // import raw fits data
    double* image = new double[in_naxes[0] * in_naxes[1]];
    if (fits_read_img(fptr, TDOUBLE, 1, in_naxes[0] * in_naxes[1], &nullval, image, &dummy_int, &status))
      printerror(status);

    // convert to float (32bits) as opencv is buggy with double (64bit)images
    float* image32 = new float[in_naxes[0] * in_naxes[1]];
    for(int j=0; j<in_naxes[0]; j++)
      for(int i=0; i<in_naxes[1]; i++)
	image32[i+j*in_naxes[1]] = (float)image[i+j*in_naxes[1]];
    
    delete[] image;

    // image32 buffer is used to construct a 32 bits cv::mat
    Mat src = Mat(Size(in_naxes[0], in_naxes[1]),CV_32FC1, image32);
    
    // we then copy the data to the final cv:mat array so we can delete image32 afterwards 
    Mat dst;
    src.copyTo(dst);
    //   src.release();
    delete[] in_naxes;
    delete[] image32;
    return dst;
}

int main (int argc, char **argv) {

  struct timeval start_time;
  struct timeval end_time;
  double start_dtime, end_dtime, diff;
  gettimeofday(&start_time,NULL);
   

  // Minimum metric value will be stored in res
  float res;

  // Integer pixel range and subpixel step ("subrange")
  int range = 0;
  float subrange = 1;

  // Creating Objects of Metrics - Normal
  calcMSE mse;
  calcSSIM ssim;
  calcPSNR psnr;
  calcMSSSIM msssim;
  calcQualityIndex iqi;
  SimilarityMetric* method;
  // Creating ouput file storage
  CvFileStorage* fs;
  char output_file[50];
  int err;
  int out_status = 0;
  fs = NULL;
  if (out_status == 1)
    fs = cvOpenFileStorage(output_file, 0, CV_STORAGE_WRITE);
  char error[100];

  // Setting up the options
  int c;
  int algo = 99; // default value = 99 = none
  Colorspace space;
  space = GRAYSCALE; // default color space

  char img_name1[500], img_name2[500]; // more space for long directory names
  int image1_status = 0, image2_status = 0;
  int opt_mse = 1, opt_psnr = 2, opt_ssim = 4, opt_msssim = 8, opt_iqi = 16;
  static struct option long_options[] = {
      {"algorithm", 1, 0, 'm'},
      {"range", 1, 0, 'r'},
      {"subrange", 1, 0, 'p'},
      {"L", 1, 0, 'L'},
      {"K1", 1, 0, '1'},
      {"K2", 1, 0, '2'},
      {"gaussian_window_size", 1, 0, 'w'},
      {"sigma", 1, 0, 's'},
      {"level", 1, 0, 'l'},
      {"alpha", 1, 0, 'a'},
      {"beta", 1, 0, 'b'},
      {"gamma", 1, 0, 'g'},
      {"B", 1, 0, 'B'},
      {"out", 1, 0, 'o'},
      {"image1", 1, 0, 1},
      {"image2", 1, 0, 2},
      {"help", 0, 0, 'h'},
      {"index_map", 0, 0, 'i'}, 
      {NULL, 0, NULL, 0}
  };
  int option_index = 0;
  int index_map = 0;
  int ind = 0;
  char * cut;


  if (argc<7) {
    cout<<"Error : Not enough input arguments\n";
    print_help_menu();
    exit(0);
  }

  while ((c = getopt_long(argc, argv, "m:r:p:L:1:2:s:l:a:b:g:B:o:hi", long_options, &option_index)) != -1) {
      
    int this_option_optind = optind ? optind : 1;
    switch (c) {
      
      case 'm':
        char algorithm[20];
        sscanf(optarg, "%s", algorithm);
          #ifdef DEBUG
          printf("Algorithm - %s \n", algorithm);
          #endif
          if (strcmp(algorithm,"mse") == 0)
	    { 
	      algo = ALGO_MSE;
	      method = (SimilarityMetric*) &mse;
	    }
          if (strcmp(algorithm,"psnr") == 0)
	    { 
	      algo = ALGO_PSNR;
	      method = (SimilarityMetric*) &psnr;
	    }
          if (strcmp(algorithm,"ssim") == 0)
	    {
              algo = ALGO_SSIM;
	      method = (SimilarityMetric*) &ssim;
	    }
          if (strcmp(algorithm,"msssim") == 0)
	    {
              algo = ALGO_MSSSIM;
	      method = (SimilarityMetric*) &msssim;
	    }
          if (strcmp(algorithm,"iqi") == 0)
            {  algo = ALGO_IQI;
	      method = (SimilarityMetric*) &iqi;
	    }
          if (algo == 99) {
              sprintf(error, "%s%s", "Invalid algorithm\n", "Possible arguments are - mse, psnr, ssim, msssim, iqi");
              printError(fs, error, out_status);
              exit(0);
          }
          break;

    case 'r':
      sscanf(optarg, "%d", &range);
      break;
      
      
    case 'p':
      sscanf(optarg, "%f", &subrange);
      if(subrange > 1) subrange = 1;
      break;
      

    case 'o':
      sscanf(optarg, "%s", output_file );
      out_status = 1;

          break;

      case 1:
          sscanf(optarg, "%s", img_name1);
          #ifdef DEBUG
    		  printf("Image1 : %s\n", img_name1);
          #endif
          image1_status = 1;
          break;
      
      case 2:
          sscanf(optarg, "%s", img_name2);
          #ifdef DEBUG
    	  	printf("Image2 : %s\n", img_name2);
          #endif
          image2_status = 1;
          break;
      
      case 'L':
          int L;
          sscanf(optarg, "%d", &L);
          psnr.setL(L);
          ssim.setL(L);
          msssim.setL(L);
          //M.setL(L);
	  // S.setL(L);
	  // MS.setL(L);
          break;
      
      case '1':
          double K1;
          sscanf(optarg, "%lf", &K1);
          #ifdef DEBUG
          printf("Setting K1 value = %lf\n", K1);
          #endif
          ssim.setK1(K1);
          msssim.setK1(K1);
          break;

      case '2':
          double K2;
          sscanf(optarg, "%lf", &K2);
          ssim.setK2(K2);
          msssim.setK2(K2);
          break;
      
      case 'w':
          int w;
          sscanf(optarg, "%d", &w);
          #ifdef DEBUG
          printf("Setting gaussian window = %d\n", w);
          #endif
          if ( w%2 == 0)
            w++;
          ssim.setGaussian_window(w);
          msssim.setGaussian_window(w);
          break;
      
      case 's':
          double sigma;
          sscanf(optarg, "%lf", &sigma);
          #ifdef DEBUG
          printf("Setting gaussian sigma = %lf\n", sigma);
          #endif
          ssim.setGaussian_sigma(sigma);
          msssim.setGaussian_sigma(sigma);
          break;
      
      case 'l':
          int level;
          sscanf(optarg, "%d", &level);
          #ifdef DEBUG
          printf("Setting level = %d\n", level);
          #endif
          msssim.setLevel(level);
          break;
      
      case 'a':
          char alpha_t[100];
          float alpha[50];
          ind = 0;
          sscanf(optarg, "%s", alpha_t);
          cut = strtok(alpha_t, ",");
          while (cut != NULL) {
            sscanf(cut, "%f", &alpha[ind]);
            cut = strtok(NULL, ",");
            ind++;
          }
          msssim.setAlpha(alpha);
          break;
      
      case 'b':
          char beta_t[100];
          float beta[50];
          ind =0;
          sscanf(optarg, "%s", beta_t);
          cut = strtok(beta_t, ",");
          while (cut != NULL) {
            sscanf(cut, "%f", &beta[ind]);
            cut = strtok(NULL, ",");
            ind++;
          }
          msssim.setBeta(beta);
          break;
 
      case 'g':
          char gamma_t[100];
          float gamma[50];
          ind =0;
          sscanf(optarg, "%s", gamma_t);
          cut = strtok(gamma_t, ",");
          while (cut !=NULL) {
            sscanf(cut, "%f", &gamma[ind]);
            cut = strtok(NULL, ",");
            ind++;
          }
          msssim.setGamma(gamma);
          break;

      case 'B':
          int B;
          sscanf(optarg, "%d", &B);
          if (B%2==1)
            B++;
          iqi.setB(B);
          break;
      
      case 'i':
          index_map =1; 
          break;
      
      case 'h':
          print_help_menu();
          break;

      case '?':
          break;
  
      default:
          print_help_menu();
          break;

    }
  }
 
 
  if (out_status == 1)
    fs = cvOpenFileStorage(output_file, 0, CV_STORAGE_WRITE);

  if (!image1_status || !image2_status) {
    sprintf(error, "%s", "No Input image Arguments");
    printError(fs, error, out_status);
    exit(0);
  }

  // Loading the source images in src1 and src2
  // in this version, the FITS images

  Mat img1 = cvFITS(img_name1);
  Mat img2 = cvFITS(img_name2);

  if ( (img1.rows != img2.rows) || (img1.cols != img2.cols) ) {
    printError(fs, "Image Dimensions mis-match", out_status);
    if (fs != NULL)
      cvReleaseFileStorage( &fs);
    exit(0);
  }
  
  float dx, dy, dx_int, dy_int, metric_value;
  Mat img2_shifted_int = registration(img1, img2, metric_value, dx_int, dy_int, range, 1., algo, method);
  Mat img2_shifted = registration(img1, img2_shifted_int, metric_value, dx, dy, 2, subrange, algo, method);
  dx += dx_int;
  dy += dy_int;

  printf("Algorithm\tRange\tSubpixel\tMinMetric\tDX\t\tDY\n");
  printf("%s\t\t%d\t%f\t%f\t%f\t%f\n", algo_names[algo], range, subrange,metric_value, dx, dy);
  waitKey(0);

  // Releasing storage
  if (fs != NULL)
    cvReleaseFileStorage( &fs);
 
  exit(0);

}

Mat registration(const Mat& img1, const Mat& img2, float& metric_value, float& dx, float& dy, const int range, const float subrange, const int algo, SimilarityMetric* method)
{
  calcMSE* mse;
  calcPSNR* psnr;
  calcSSIM* ssim;
  calcMSSSIM* msssim;
  calcQualityIndex* iqi; 
   switch(algo)
    {
    case ALGO_MSE:
      mse = dynamic_cast<calcMSE*> (method); 
       break;
    case ALGO_PSNR:
      psnr = dynamic_cast<calcPSNR*> (method); 
      break;
    case ALGO_SSIM:
      ssim = dynamic_cast<calcSSIM*> (method);
      break;
    case ALGO_MSSSIM:
      msssim = dynamic_cast<calcMSSSIM*> (method);
      break;
    case ALGO_IQI:
      iqi = dynamic_cast<calcQualityIndex*> (method);
      break;
    }
 
  const float shift_range_x = (float) range;
  const float shift_range_y = (float) range;
  const float step_x = subrange;
  const float step_y = subrange;
  Mat translation;    
  Mat img2_shifted; // stores shifted versions of img2
  float res, bestres = 1e99, bestx=0, besty=0;
  for(float tx=-shift_range_x; tx<=shift_range_x;tx+=step_x)
    {
      for(float ty=-shift_range_y; ty<=shift_range_y;ty+=step_y)
	{
	 
	  // set the translation matrix
	  translation = (Mat_<float>(2,3) << 1, 0, tx, 0, 1, ty);
	  // shift the images
	  warpAffine(img2, img2_shifted,translation,img2.size());

	  switch(algo)
	    {
	    case ALGO_MSE:
	      res = mse->compare(img1, img2_shifted);
	      break;
	    case ALGO_PSNR:
	      res = psnr->compare(img1, img2_shifted);
	      break;

	    case ALGO_SSIM:
	      res = ssim->compare(img1, img2_shifted);
	      break;
	    case ALGO_MSSSIM:
	      res = msssim->compare(img1, img2_shifted);
	      break;
	    case ALGO_IQI:
	      res = iqi->compare(img1, img2_shifted);
	      break;
	    }

	  if(res<bestres)
	    {
	      bestres = res;
	      bestx   = tx;
	      besty   = ty;
	    }
	  //cout << tx << " " << ty << " " << res << "\n"; 
	}
      
    }

  metric_value = bestres;
  dx = bestx;
  dy = besty;
  // Now do the final transformation
  translation = (Mat_<float>(2,3) << 1, 0, bestx, 0, 1, besty);
  warpAffine(img2, img2_shifted,translation,img2.size());

  return img2_shifted;
}
