#include <stdlib.h>
#include <sstream>
#include <ctime>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <getopt.h>    // for getopt_long
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)
using namespace cv;
using namespace std;

// Including core.h
#include "core.h"

// Including the Similarity Metric Header files
#include "similarityMetric.h"
#include "mse.h"
#include "psnr.h"
#include "ssim.h"
#include "msssim.h"
#include "iqi.h"

// FITS library
#include "fitsio.h"


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


cv::Mat cvFITS(const char *filename)
{
    // A lot of ugly steps here...

    fitsfile *fptr; /* pointer to FITS files, defined in fitsio.h */
    int status, dummy_int, nfound;
    char dummy_char[10];
    long k;
    double nullval = 0.;
    if (fits_open_file(&fptr, filename, READONLY, &status))
      printerror(status);

    fits_read_key_lng(fptr, "NAXIS", &k, dummy_char, &status);
    long* in_naxes = new long[2];

    if (fits_read_keys_lng(fptr, "NAXIS", 1, 2, in_naxes, &nfound, &status))
      printerror(status);

    //  printf("FITS image size: %ld x %ld - %d\n",in_naxes[0] , in_naxes[1], nfound );

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
    cv::Mat src = cv::Mat(cv::Size(in_naxes[0], in_naxes[1]),CV_32FC1, image32);
    
    // we then copy the data to the final cv:mat array so we can delete image32 afterwards 
    cv::Mat dst;
    src.copyTo(dst);

    delete[] in_naxes;
    delete[] image32;
    return dst;
}

int main (int argc, char **argv) {

  struct timeval start_time;
  struct timeval end_time;
  double start_dtime, end_dtime, diff;
  gettimeofday(&start_time,NULL);
   

  // Result will be stored in res
  float res;
  // Creating Objects of Metrics - Normal
  calcMSE mse;
  calcSSIM ssim;
  calcPSNR psnr;
  calcMSSSIM msssim;
  calcQualityIndex iqi;

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
  int algo = 0;
  Colorspace space;
  space = GRAYSCALE; // default color space

  char img_name1[500], img_name2[500]; // more space for long directory names
  int image1_status = 0, image2_status = 0;
  int opt_mse = 1, opt_psnr = 2, opt_ssim = 4, opt_msssim = 8, opt_iqi = 16;
  static struct option long_options[] = {
      {"algorithm", 1, 0, 'm'},
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

  while ((c = getopt_long(argc, argv, "m:L:1:2:s:l:a:b:g:B:o:hi", long_options, &option_index)) != -1) {
      
    int this_option_optind = optind ? optind : 1;
    switch (c) {
      
      case 'm':
        char algorithm[20];
        sscanf(optarg, "%s", algorithm);
          #ifdef DEBUG
          printf("Algorithm - %s \n", algorithm);
          #endif
          if (strcmp(algorithm,"mse") == 0)
              algo = algo | opt_mse;
          if (strcmp(algorithm,"psnr") == 0)
              algo = algo | opt_psnr;
          if (strcmp(algorithm,"ssim") == 0)
              algo = algo | opt_ssim;
          if (strcmp(algorithm,"msssim") == 0)
              algo = algo | opt_msssim;
          if (strcmp(algorithm,"iqi") == 0)
              algo = algo | opt_iqi;
          if (strcmp(algorithm,"all") == 0)
              algo = opt_mse | opt_psnr | opt_ssim | opt_msssim | opt_iqi;
          if (!algo) {
              sprintf(error, "%s%s", "Invalid algorithm\n", "Possible arguments are - mse, psnr, ssim, msssim, iqi, all");
              printError(fs, error, out_status);
              exit(0);
          }
          break;
      
      case 'o':
          sscanf(optarg, "%s", output_file );
          out_status = 1;
          #ifdef DEBUG
          printf("output_file - %s \n", output_file);
          #endif
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
          #ifdef DEBUG
          printf("Setting L value = %d\n", L);
          #endif
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
	  // S.setK1(K1);
	  // MS.setK1(K1);
          break;

      case '2':
          double K2;
          sscanf(optarg, "%lf", &K2);
          #ifdef DEBUG
          printf("Setting K2 value = %lf\n", K2);
          #endif
          ssim.setK2(K2);
          msssim.setK2(K2);
          //S.setK2(K2);
	  // MS.setK2(K2);
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
          //S.setGaussian_window(w);
	  // MS.setGaussian_window(w);
          break;
      
      case 's':
          double sigma;
          sscanf(optarg, "%lf", &sigma);
          #ifdef DEBUG
          printf("Setting gaussian sigma = %lf\n", sigma);
          #endif
          ssim.setGaussian_sigma(sigma);
          msssim.setGaussian_sigma(sigma);
          //S.setGaussian_sigma(sigma);
	  // MS.setGaussian_sigma(sigma);
          break;
      
      case 'l':
          int level;
          sscanf(optarg, "%d", &level);
          #ifdef DEBUG
          printf("Setting level = %d\n", level);
          #endif
          msssim.setLevel(level);
          //MS.setLevel(level);
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
          #ifdef DEBUG
          printf("Setting alpha = \n");
          #endif
          msssim.setAlpha(alpha);
          //MS.setAlpha(alpha);
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
          #ifdef DEBUG
          printf("Setting beta = \n");
          #endif
          msssim.setBeta(beta);
	  // MS.setBeta(beta);
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
          #ifdef DEBUG
          printf("Setting gamma = \n");
          #endif
          msssim.setGamma(gamma);
          //MS.setGamma(gamma);
          break;

      case 'B':
          int B;
          sscanf(optarg, "%d", &B);
          #ifdef DEBUG
          printf("Setting B = %d\n", B);
          #endif
          if (B%2==1)
            B++;
          iqi.setB(B);
          //I.setB(B);
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

  int shift_range_x = 10; //integer registration range
  int shift_range_y = 10;
  Mat translation;    
  Mat img2_shifted; // shifted versions of img1 and img2

 float bestres = 1e99;
 int bestx=0, besty=0;
 float fullres[2*shift_range_x+1][2*shift_range_y+1];

  for(int i=-shift_range_x;i<=shift_range_x;i++)
    {
      for(int j=-shift_range_y;j<=shift_range_y;j++)
	{
	  // set the translation matrix
	  translation = (Mat_<float>(2,3) << 1, 0, (float)i, 0, 1, (float)j);
	  // shift the images
	  warpAffine(img2, img2_shifted,translation,img2.size());
	  //	  cout << i << " " << j << " ";
	  //writeMatToFile(src1, "shifted.txt");
	  //getchar(); 
	  if ((algo & opt_mse) != 0)
	    {
	      res = mse.compare(img1, img2_shifted);
	      //  cout << "MSE:\t" << res << "\n";
	    } 

	  if ((algo & opt_psnr) != 0)
	    {
	      res = psnr.compare(img1, img2_shifted);
	      // cout << "PSNR:\t" << res << "\n";
	    }
	  
	  if ((algo & opt_ssim) != 0)
	    {
	      res = ssim.compare(img1, img2_shifted);
	      // cout << "D-SSIM:\t" << res << "\n";
	    }
	  
	  if ((algo & opt_msssim) != 0)
	    {
	      res = msssim.compare(img1, img2_shifted);
	      // cout << "D-MS-SSIM:\t" << res << "\n";
	    }
	  
	  if ((algo & opt_iqi) != 0)
	    {
	      res = iqi.compare(img1, img2_shifted);
	      // cout << "IQI:\t" << res << "\n";
	    }

	  fullres[i+shift_range_x][j+shift_range_y] = res;


      if(res<bestres)
	{
	  bestres=res;
	  bestx = -i;
	  besty = -j;
	}

	}
      
    }
  cout << bestres << "\t"<< bestx <<"\t" << besty << endl;

  waitKey(0);

  // Releasing storage
  if (fs != NULL)
    cvReleaseFileStorage( &fs);
 
  exit(0);

}
