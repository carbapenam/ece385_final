from PIL import Image
from bitstring import BitArray
import sys
import struct
import math

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
        line = line.strip()
        if line == "[Background]":
            flag = 'B'
            print("processing background")
            continue    
        if line == "[Character]":
            flag = 'C'
            print("processing character")
            continue
        if line == "[Font]":
            flag = 'F'
            print("processing font")
            continue
        if line == "[Script]":
            flag = 'S'
            print("processing script")
            continue
            
        if flag == 'B':
            convert_generic_image(line, background)
            continue
        
        if flag == 'F':
            convert_generic_image(line, font)
            continue
            
        if flag == 'C':
            temp = line.split()
            convert_chara(temp[0], int(temp[1]), int(temp[2]), chara)
            continue
            
        if flag == 'S':
            convert_text(line, text)
            continue
    assemble(background, chara, font, text, output)
    
def assemble(background, chara, font, text, file_handle):
    addr_offset = 0
    
    file_handle.write('B'.encode()+b'\x00\x00\x00')
    #start with background
    for bg in background:
        file_handle.write(struct.pack("<I", bg.width))
        file_handle.write(struct.pack("<I", bg.height))
        file_handle.write(struct.pack("<I", addr_offset))
        addr_offset = addr_offset + (bg.width*bg.height)
    
    #chara
    file_handle.write('C'.encode()+b'\x00\x00\x00')
    for c in chara:
        file_handle.write(struct.pack("<I", c.width))
        file_handle.write(struct.pack("<I", c.height))
        file_handle.write(struct.pack("<I", c.offset_x))
        file_handle.write(struct.pack("<I", c.offset_y))
        file_handle.write(struct.pack("<I", addr_offset))
        
        addr_offset = addr_offset + (c.width * c.height)
    
    #font
    file_handle.write('F'.encode()+b'\x00\x00\x00')
    for f in font:
        file_handle.write(struct.pack("<I", f.width))
        file_handle.write(struct.pack("<I", f.height))
        file_handle.write(struct.pack("<I", addr_offset))
        
        addr_offset = addr_offset + (f.width * f.height)
    
    #text
    file_handle.write('T'.encode()+b'\x00\x00\x00')
    for t in text:
        file_handle.write(struct.pack("<I", t.length))
        file_handle.write(struct.pack("<I", addr_offset))
        
        addr_offset = addr_offset + t.length
    
    #data section
    file_handle.write('D'.encode()+b'\x00\x00\x00')
    for bg in background:
        file_handle.write(bg.image)
    for c in chara:
        file_handle.write(c.image)
    for f in font:
        file_handle.write(f.image)
    for t in text:
        file_handle.write(t.text)
        
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
            
            buf = buf + struct.pack("<I", temp)
    return Generic_Image(width, height, 0, buf)

def convert_generic_image(file_name, image_list):
    print("processing: "+file_name)
    image_list.append(return_generic_image(file_name)) 
    
def convert_chara(file_name, offset_x, offset_y, chara_list):
    print("processing: "+file_name)
    temp = return_generic_image(file_name)
    chara_list.append(Chara(temp.width, temp.height, offset_x, offset_y, 0, temp.image))
    
def convert_text(line, text_list):
    buf = line.encode() + b'\0'
    zeropad_len = 4 * math.ceil(len(buf) / 4.0) - len(buf) 
    padded_len = math.ceil(len(buf) / 4.0)
    print(zeropad_len, len(buf))    
    #zero pad the strings
    buf = buf + b'\0' * zeropad_len

    text_list.append(Text(padded_len, 0, buf))
    
if __name__ == "__main__":
    if len(sys.argv) == 3:
        batch = open(sys.argv[1], "r", encoding="utf8")
        output = open(sys.argv[2], "wb")
        
        convert(batch, output)
        output.close()
        batch.close()
        
    else:
        print("check your arguments.\n usage: convert.py [input png filename] [output ram file]\n")