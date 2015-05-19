function splash, refimage, currentimage, metric = metric, range = range, subrange = subrange, k1 = k1, k2 = k2, l = l,gaussian_window_size =gaussian_window_size, sigma =sigma, level = level, alpha = alpha , beta = beta, gamma = gamma ,  b =b , dx = dx, dy = dy
;
;
;  
; IN (mandatory):  refimage, currentimage: filenames of FITS files to compare 
; IN (optional) :  metric  : string, algorithm used for registration, e.g. 'mse', 'ssim'
;                  range   : integer, number of pixels defining the half-range searched for registration
;                  subrange: float, fraction of pixel to which the registration is done
;                  SEE SPLASH HELP FOR OTHER KEYWORDS
; OUT           :   metric : value of the metric when images are co-registered
; OUT (optional):  dx, dy  : final shift
;
;

if (keyword_set(refimage) EQ 0) or (keyword_set(currentimage) EQ 0) then begin
print, 'ERROR in splash.pro : Missing image file' 
stop
return, 0
end

if keyword_set(metric) EQ 0 THEN metric = 'mse' ELSE BEGIN
indx = where(metric EQ ['mse', 'psnr', 'ssim', 'msssim', 'iqi'])
if(indx[0] EQ -1) then metric = 'mse'
ENDELSE

if(keyword_set(range) EQ 0) then range = 0
if(keyword_set(subrange) EQ 0) then subrange = 0

command = '/home/baron/SOFTWARE/splash/bin/splash --image1 '+refimage+' --image2 '+currentimage+' --algorithm '+metric+' --range '+strcompress(string(range), /remove_all) +' --subrange '+strcompress(string(subrange), /remove_all)

if(keyword_set(k1) NE 0) then command += ' --K1 ' + strcompress(string(k1), /remove_all)
if(keyword_set(k2) NE 0) then command += ' --K2 ' + strcompress(string(k2), /remove_all)
if(keyword_set(l) NE 0) then command += ' --l ' + strcompress(string(l), /remove_all)
if(keyword_set(gaussian_window_size) NE 0) then command += ' --gaussian_window_size '+ strcompress(string(gaussian_window_size), /remove_all)
if(keyword_set(sigma) NE 0) then command += ' --sigma ' + strcompress(string(sigma), /remove_all)
if(keyword_set(level) NE 0) then command += ' --level ' + strcompress(string(level), /remove_all)
if(keyword_set(alpha) NE 0) then command += ' --alpha ' + strcompress(string(alpha), /remove_all)
if(keyword_set(beta) NE 0) then command += ' --beta ' + strcompress(string(beta), /remove_all)
if(keyword_set(gamma) NE 0) then command += ' --gamma ' + strcompress(string(gamma), /remove_all)
if(keyword_set(b) NE 0) then command += ' --b ' + strcompress(string(b), /remove_all)

command = command +' > splash.tmp'
print, command
spawn, command
ret = (read_ascii('splash.tmp')).field1
dx = ret[4, 1]
dy = ret[5,1 ]
return, ret[3,1]
end
