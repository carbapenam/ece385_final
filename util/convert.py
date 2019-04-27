from PIL import Image
from bitstring import BitArray
import sys

# Image format: RGBA, big endian

if __name__ == "__main__":
    if len(sys.argv) == 3:
        output = open(sys.argv[2], "wb")
        im = Image.open(sys.argv[1])
        
        buf = bytearray()
        width = im.size[0]
        height = im.size[1]
        
        for y in range(0, height):
            for x in range(0, width):
                # If pixel values only have RGB, append A as 255. 
            
                pixel_value = list(im.getpixel((x,y)))
                if len(pixel_value) == 3:
                    pixel_value.append(255)
                
                
                # the complex part is coming right now
                temp = 0;
                temp = ((int(pixel_value[0]*32.0/256.0) << 27)# Red 
                + (int(pixel_value[1]*64.0/256.0) << 21) # Green 
                + (int(pixel_value[2]*32.0/256.0) << 16) # Blue
                + (int(pixel_value[3]) << 8)
                )
                
                output.write(BitArray(uintle = temp, length=32).bytes)
          
        output.close()
        
    else:
        print("check your arguments.\n usage: convert.py [input png filename] [output ram file]\n")