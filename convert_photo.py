"""
=============================================================================
Photo Converter Utility for ESP32-2432S028 Digital ID Badge
=============================================================================
Use this script if you ever want to replace your photo with a new picture!

Requirements:
    pip install pillow

Usage:
    python convert_photo.py my_new_photo.jpg
=============================================================================
"""

import sys
import os
from PIL import Image

TARGET_WIDTH = 130
TARGET_HEIGHT = 150

def convert(image_path, output_header="photo_data.h"):
    if not os.path.exists(image_path):
        print(f"Error: File '{image_path}' not found!")
        return

    print(f"Processing image: {image_path}...")
    img = Image.open(image_path).convert("RGB")
    w, h = img.size

    # Maintain 130:150 aspect ratio crop from top-center
    target_ratio = TARGET_WIDTH / TARGET_HEIGHT
    current_ratio = w / h

    if current_ratio > target_ratio:
        new_w = int(h * target_ratio)
        offset_x = (w - new_w) // 2
        img = img.crop((offset_x, 0, offset_x + new_w, h))
    else:
        new_h = int(w / target_ratio)
        img = img.crop((0, 0, w, new_h))

    img = img.resize((TARGET_WIDTH, TARGET_HEIGHT), Image.Resampling.LANCZOS)
    
    # Save optimized JPEG to buffer
    temp_jpg = "temp_id_photo.jpg"
    img.save(temp_jpg, "JPEG", quality=85, optimize=True)

    with open(temp_jpg, "rb") as f:
        jpg_bytes = f.read()
    
    if os.path.exists(temp_jpg):
        os.remove(temp_jpg)

    print(f"JPEG size: {len(jpg_bytes)} bytes")

    # Generate C header
    header_content = f"""#ifndef PHOTO_DATA_H
#define PHOTO_DATA_H

#include <Arduino.h>

#define PHOTO_WIDTH  {TARGET_WIDTH}
#define PHOTO_HEIGHT {TARGET_HEIGHT}

// Embedded JPEG data ({len(jpg_bytes)} bytes)
const uint8_t photo_jpg[] PROGMEM = {{
"""
    for i, b in enumerate(jpg_bytes):
        if i % 16 == 0:
            header_content += "    "
        header_content += f"0x{b:02X}, "
        if i % 16 == 15 or i == len(jpg_bytes) - 1:
            header_content += "\n"

    header_content += """};

#endif // PHOTO_DATA_H
"""

    with open(output_header, "w") as f:
        f.write(header_content)

    print(f"Successfully generated: {output_header}!")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        convert(sys.argv[1])
    else:
        print("Usage: python convert_photo.py <path_to_image>")
