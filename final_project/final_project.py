import cv2
import numpy as np
import matplotlib.pyplot as plt
import time
import scipy.signal as ss
import pycuda.autoinit
import pycuda.driver as cuda
from pycuda.compiler import SourceModule
img = cv2.imread("river.jpg")

img =cv2.resize(img,(1000,1000))

img = cv2.cvtColor(img,cv2.COLOR_BGR2RGB)
img_gray = cv2.cvtColor(img,cv2.COLOR_RGB2GRAY)


kernel = np.array([[-1,-2,-1],[0,0,0],[1,2,1]])










mod = SourceModule('''   

__global__ void convolutionGPU(float *d_Result,float *d_Data,float *d_Kernel ,int dataW ,int dataH )
{

    const  int   KERNEL_RADIUS=1;  
    const  int   KERNEL_W = 2 * KERNEL_RADIUS + 1;

    __shared__ float sPartials[KERNEL_W*KERNEL_W];    

     int col = threadIdx.y + blockDim.y * blockIdx.y;
     int row = threadIdx.x + blockDim.x * blockIdx.x;
     int gLoc = row + dataW*col;
     
     for(int i=0 ;  i< KERNEL_W*KERNEL_W ; i+=1 ){
         sPartials[i]= d_Kernel[i];//d_Kernel[gLoc1] ;
        }
    
     float sum = 0; 
     float value = 0;
     for(int i = -KERNEL_RADIUS; i<=KERNEL_RADIUS ; i++)
     	for(int j = -KERNEL_RADIUS; j<=KERNEL_RADIUS ;j++ ){  
          if( (col+j)<0 ||(row+i) < 0 ||(row+i) > (dataW-1) ||(col+j )>(dataH-1) )
          value = 0;
          else        
          value = d_Data[gLoc + i + j * dataH];
          sum += value * sPartials[(i+KERNEL_RADIUS) + (j+KERNEL_RADIUS)*KERNEL_W];
    }
       d_Result[gLoc] = sum;
 }
''')
       
convolutionGPU = mod.get_function("convolutionGPU") 




def convolution_cuda(sourceImage,fil):
    destImage = sourceImage.copy()
    
    (imageHeight,  imageWidth) = sourceImage.shape
    fil = np.float32(fil)
    DATA_H = imageHeight;
    DATA_W = imageWidth
    DATA_H = np.int32(DATA_H)
    DATA_W = np.int32(DATA_W)

    sourceImage_gpu = cuda.mem_alloc_like(sourceImage)
    fil_gpu = cuda.mem_alloc_like(fil)
    destImage_gpu = cuda.mem_alloc_like(sourceImage)

    cuda.memcpy_htod(sourceImage_gpu, sourceImage)
    cuda.memcpy_htod(fil_gpu,fil)

  
    convolutionGPU(destImage_gpu, sourceImage_gpu , fil_gpu,  DATA_W,  DATA_H  , block=(1000,1,1), grid=(1,1000))

    cuda.memcpy_dtoh(destImage, destImage_gpu)
    return destImage




def test_convolution_cuda(img,kernel):

    original = img
    original = np.float32(original)
 
    fil = kernel

    destImage = original.copy()
    destImage[:] = np.nan
    destImage = convolution_cuda(original,  fil)
    return destImage*-1







start_time = time.time()

result = ss.convolve2d(img_gray,kernel,mode="same")
end_time = time.time()

spend_time =round(end_time-start_time,6)*1000



start_time2 = time.time()


result2 = test_convolution_cuda(img_gray,kernel)

end_time2 = time.time()

spend_time2 =round(end_time2-start_time2,6)*1000

fig = plt.figure(dpi=100)


ax3 = fig.add_subplot(1,3,1)
ax3.imshow(img)
ax3.set_title("Original Image")

ax1 = fig.add_subplot(1,3,2)
ax1.imshow(result,cmap="gray")
ax1.set_title(f"Using CPU spend times \n{spend_time} ms")


ax2 = fig.add_subplot(1,3,3)
ax2.imshow(result2,cmap="gray")
ax2.set_title(f"Using GPU spend times \n{spend_time2} ms")




print("Speedup %f",spend_time/spend_time2)


plt.show()
plt.close()