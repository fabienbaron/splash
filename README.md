SPLASH is a FITS image comparison software licensed under the GNU
GENERAL PUBLIC LICENSE Version 3.

SPLASH was originally based on metric, a Multi-threaded Image
Similarity Metric Library used to compare two images. metric was a
part of project DIFFER as a part of Google Summer of Code
(GSoC-2012).

Compared to the original code:
- SPLASH uses modern openCV > 2.4.x classes.
- SPLASH uses 32 bit floating point image representations.
- SPLASH

- Mean Square Error(MSE),
- Peak Signal to Noise Ratio(PSNR)
- SSIM, see e.g.  https://ece.uwaterloo.ca/~z70wang/publications/ssim.html
- MS-SSIM, see e.g. https://ece.uwaterloo.ca/~z70wang/publications/msssim.pdf
- Universal Image Quality Index, Z. Wang, A.C. Bovik, "A Universal Image Quality Index", IEEE Signal Processing Letters, vol.9, no.3, pp.81-84, Mar 2002.

Acknowledgments
======================

The original work on DIFFER and metric was supported as a part
of Google Summer of code (GSoC-2012), the project Mentor being Vaclav
Rosecky (xrosecky@gmail.com), and the developer Dushyant Goyal
(goyal1dushyant@gmail.com).

