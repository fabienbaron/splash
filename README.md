SPLASH is a FITS image registration and comparison software licensed under the GNU
GENERAL PUBLIC LICENSE Version 3, currently developped by Fabien Baron
(baron@chara.gsu.edu).

SPLASH was originally based on metric, a Multi-threaded Image
Similarity Metric Library used to compare two images. metric was a
part of project DIFFER as a part of Google Summer of Code
(GSoC-2012).

Compared to the original code:
- SPLASH uses the modern cv::Mat class.
- SPLASH uses single-channel 32-bit image representations.
- SPLASH takes FITS images as input
- SPLASH uses the CMake installation framework.

SPLASH computes the following difference metrics between two FITS images:
- Mean Square Error(MSE),
- Peak Signal to Noise Ratio(PSNR)
- SSIM, see e.g.  https://ece.uwaterloo.ca/~z70wang/publications/ssim.html
- MS-SSIM, see e.g. https://ece.uwaterloo.ca/~z70wang/publications/msssim.pdf
- Universal Image Quality Index, Z. Wang, A.C. Bovik, "A Universal Image Quality Index", IEEE Signal Processing Letters, vol.9, no.3, pp.81-84, Mar 2002.
- in progress: Kullback-Leibler divergence

Requirements
======================

SPLASH uses OpenCV. 
Under Ubuntu, libopencv-dev and libopencv-highgui-dev are required.

Installation
======================

SPLASH uses the classic CMake installation procedure. 


Acknowledgments
======================

The original work on DIFFER and metric was supported as a part
of Google Summer of code (GSoC-2012), the project Mentor being Vaclav
Rosecky (xrosecky@gmail.com), and the developer Dushyant Goyal
(goyal1dushyant@gmail.com).

