import numpy
import pycuda.autoinit
import pycuda.driver as cuda
from pycuda.compiler import SourceModule
import scipy.signal as ss

mod = SourceModule('''   

__global__ void convolutionGPU(float *d_Result,float *d_Data,float *d_Kernel ,int dataW ,int dataH )
{

    const  int   KERNEL_RADIUS=1;  
    const  int   KERNEL_W = 2 * KERNEL_RADIUS + 1;

    __shared__ float sPartials[KERNEL_W*KERNEL_W];    

     int col = threadIdx.y + blockDim.y * blockIdx.y;
     int row = threadIdx.x + blockDim.x * blockIdx.x;
     int gLoc = row + dataW*col;
     
     for(int i=0 ;  i< KERNEL_W*KERNEL_W ; i+=1 )
     sPartials[i]= d_Kernel[i];//d_Kernel[gLoc1] ;
    
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
    # Perform separable convolution on sourceImage using CUDA.
    destImage = sourceImage.copy()
    
    (imageHeight,  imageWidth) = sourceImage.shape
    fil = numpy.float32(fil)
    DATA_H = imageHeight;
    DATA_W = imageWidth
    DATA_H = numpy.int32(DATA_H)
    DATA_W = numpy.int32(DATA_W)
    # Prepare device arrays

    sourceImage_gpu = cuda.mem_alloc_like(sourceImage)
    fil_gpu = cuda.mem_alloc_like(fil)
    destImage_gpu = cuda.mem_alloc_like(sourceImage)

    cuda.memcpy_htod(sourceImage_gpu, sourceImage)
    cuda.memcpy_htod(fil_gpu,fil)

    print ('star')
    convolutionGPU(destImage_gpu, sourceImage_gpu , fil_gpu,  DATA_W,  DATA_H  , block=(1000,1,1), grid=(1,00))
    # Pull the data back from the GPU.
    cuda.memcpy_dtoh(destImage, destImage_gpu)
    return destImage


def test_convolution_cuda(img,kernel):
    # Test the convolution kernel.
    # Generate or load a test image

    original = numpy.float32(img)
    print ("original =",original)
    # You probably want to display the image using the tool of your choice here.
    fil = kernel

    destImage = original.copy()
    destImage[:] = numpy.nan
    destImage = convolution_cuda(original,  fil)
    # You probably wand to display the result image using the tool of your choice here.
    print ('Done running the convolution kernel!')
    print ( destImage)

img=numpy.random.randint(0,255,(1000,1000))
kernel = numpy.array([[1,0,1],[0,1,0],[1,0,1]])

# start_time = time.time()

test_convolution_cuda(img,kernel)
sobelY = ss.convolve2d(img,kernel,mode="same")
print(sobelY)