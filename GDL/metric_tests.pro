; Simple test to check for metric values
; Convolution of Red Supergiant image with 0-10 pixels Gaussian
;
ref_name ='../samples/stellar-surface_ref-img.fits'
ref = readfits(ref_name)
im_name = 'test_im.fits'
nblur = 10
mse = dblarr(nblur)
;psnr = dblarr(nblur)
ssim = dblarr(nblur)
msssim = dblarr(nblur)
iqi = dblarr(nblur)
k1 = 1E-9
k2 = 3E-9
sigma = 1.5
gaussian_window_size = 5
for i = 0, nblur-1 do begin
   im = GAUSS_SMOOTH(ref, i,/EDGE_ZERO)
 ;  fbim, im
 ;  stop
   writefits, im_name, im
;   mse[i] = splash(ref_name, im_name, metric='mse', range = 0, subrange = 0)
;   psnr[i]= splash(ref_name, im_name, metric='psnr', range = 0, subrange = 0)
   ssim[i] = splash(ref_name, im_name, metric='ssim', range = 0, subrange = 0, k1 = k1, k2 = k2, sigma = sigma, gaussian_window_size = gaussian_window_size)
  ; msssim[i] = splash(ref_name, im_name, metric='msssim', range = 0, subrange = 0, k1 = k1, k2 = k2, sigma = sigma, gaussian_window_size = gaussian_window_size)
   iqi[i] = splash(ref_name, im_name, metric='iqi', range = 0, subrange = 0)
endfor
plot,  msssim, /psym
print, msssim
stop
end
