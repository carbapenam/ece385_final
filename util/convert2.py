from PIL import Image
from bitstring import BitArray
import sys

# Image format: RGBA, big endian
class bg():
    def __init__(self, width, height, addr):
        self.width = width
        self.height = height
        self.addr = addr

class chara():
    def __init__(self, width, height, offset_x, offset_y, addr):
        self.width = width
        self.height = height
        self.offset_x = offset_x
        self.offset_y = offset_y
        self.addr = addr
        
class font():
    def __init__(self, width, height, addr):
        self.width = width
        self.height = height
        self.addr = addr

class text():
    def __init__(self, length, addr):
        self.length = length
        self.addr = addr

def convert():
    

        
def write_image(file_name):
    im = Image.open(file_name)
    
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
            
    return BitArray(uintle = temp, length=32).bytes

def write_text
             
if __name__ == "__main__":
    if len(sys.argv) == 3:
        batch = open(sys.argv[1], "r")
        output = open(sys.argv[2], "wb")
        
        convert(batch, output)
        output.close()
        
    else:
        print("check your arguments.\n usage: convert.py [input png filename] [output ram file]\n")
