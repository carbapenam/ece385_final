from PIL import Image
from bitstring import BitArray
import sys

# Image format: RGBA, little endian
class Generic_Image():
    def __init__(self, width, height, addr, image):
        self.width = width
        self.height = height
        self.addr = addr
        self.image = image

class Chara():
    def __init__(self, width, height, offset_x, offset_y, addr, image):
        self.width = width
        self.height = height
        self.offset_x = offset_x
        self.offset_y = offset_y
        self.addr = addr
        self.image = image

class Text():
    def __init__(self, length, addr, text):
        self.length = length
        self.addr = addr
        self.text = text
        
def convert(batch, output):
    background = []
    chara = []
    font = []
    text = []
    
    flag = '' #B, C, F, T 
    for line in batch:
        if line == "[Background]":
            flag = 'B'
            continue
        if line == "[Character]":
            flag = 'C'
            continue
        if line == "[Font]":
            flag = 'F'
            continue
        if line == "[Script]":
            flag = 'S'
            continue
            
        if flag == 'B':
            convert_generic_image(line, background)
            continue
        
        if flag == 'F':
            convert_generic_image(line, font)
            continue
            
        if flag == 'C':
            #parse the offset_info
            convert_chara(line, chara)
            continue
            
        if flag == 'S':
            #parse quotation
            convert_text(line, text)
            continue
    
    assemble(background, chara, font, text, output)
    
def assemble(background, chara, font, text, file_handle):
    offset = 0
    
    #start with background
    for bg in background:
        
        offset = offset + width*height
    
    #chara
    
    #font
    
    #text
    
    
    
    #where the file is made
    
def return_generic_image(file_name):
    im = Image.open(file_name)
    
    width = im.size[0]
    height = im.size[1]
    buf = b''
    
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
            
            buf = buf + BitArray(uintle = temp, length=32).bytes
    return Generic_Image(width, height, 0, buf)

def convert_generic_image(file_name, image_list):
    image_list.append(return_generic_image(file_name)) 
    
def convert_chara(file_name, offset_x, offset_y, chara_list):
    temp = return_generic_image(file_name)
    chara_list.append(temp.width, temp.height, offset_x, offset_y, 0, temp.image)
    
def convert_text(line, text_list)
    buf = line.encode() + b'\x00'
    zeropad_len = math.ceil(len(buf) / 4.0) - len(buf) 
    padded_len = math.ceil(len(buf) / 4.0)
    
    #zero pad the strings
    buf = buf + b'\x00'*zeropad_len
    text_list.append(Text(padded_len, 0, buf))
    
if __name__ == "__main__":
    if len(sys.argv) == 3:
        batch = open(sys.argv[1], "r")
        output = open(sys.argv[2], "wb")
        
        convert(batch, output)
        output.close()
        
    else:
        print("check your arguments.\n usage: convert.py [input png filename] [output ram file]\n")
