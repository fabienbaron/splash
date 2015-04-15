SPLASH is a FITS image registration and comparison software licensed under the GNU
GENERAL PUBLIC LICENSE Version 3, currently developped by Fabien Baron
(baron@chara.gsu.edu).

SPLASH computes the following difference metrics between two FITS images:
- Mean Square Error(MSE),
- Peak Signal to Noise Ratio(PSNR)
- SSIM, see e.g.  https://ece.uwaterloo.ca/~z70wang/publications/ssim.html
- MS-SSIM, see e.g. https://ece.uwaterloo.ca/~z70wang/publications/msssim.pdf
- Universal Image Quality Index, Z. Wang, A.C. Bovik, "A Universal Image Quality Index", IEEE Signal Processing Letters, vol.9, no.3, pp.81-84, Mar 2002.
- (in progress) Kullback-Leibler divergence

Requirements
======================

SPLASH uses OpenCV. 
Under Ubuntu, the library libopencv-dev is required.

Installation
======================

SPLASH uses the classic CMake installation procedure. 


Acknowledgments
======================

SPLASH was originally based on metric/DIFFER, developped for Google
Summer of Code (GSoC-2012), for which the project Mentor was Vaclav
Rosecky (xrosecky@gmail.com), and the developer Dushyant Goyal
(goyal1dushyant@gmail.com).

Compared to the original code:
- SPLASH does image registration.
- SPLASH uses the modern cv::Mat class.
- SPLASH uses single-channel 32-bit image representations.
- SPLASH takes FITS images as input
- SPLASH uses the CMake installation framework.
