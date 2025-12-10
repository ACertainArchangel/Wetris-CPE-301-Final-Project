import os
from PIL import Image, ImageOps

# ==== SETTINGS ====
# Format: (left, top, right, bottom)
global_crop = (0, 0, 0, 00)  # Example: crop 500px from top only

# Override crop values for specific files if needed
per_file_crop = {
    # "example.jpg": (100, 0, 100, 50)
}

# Apply EXIF orientation and remove the tag if True
fix_orientation = True

# Overwrite original files instead of saving into "cropped/"
overwrite = True

# ==== OUTPUT DIRECTORY (ignored if overwrite=True) ====
output_dir = "cropped"
if not overwrite:
    os.makedirs(output_dir, exist_ok=True)

# ==== PROCESS ALL .JPG FILES ====
for filename in os.listdir("."):
    if filename.lower().endswith(".jpg") or filename.lower().endswith(".jpg"):
        img = Image.open(filename)

        if fix_orientation:
            # Rotate/flip pixel data to match EXIF orientation
            img = ImageOps.exif_transpose(img)
            # Strip EXIF metadata entirely so orientation tag is gone
            data = list(img.getdata())
            img_no_exif = Image.new(img.mode, img.size)
            img_no_exif.putdata(data)
            img = img_no_exif

        width, height = img.size

        # Pick crop settings
        crop_values = per_file_crop.get(filename, global_crop)
        left, top, right, bottom = crop_values

        # Compute crop box
        crop_box = (left, top, width - right, height - bottom)

        # Ensure valid crop region
        if crop_box[0] < crop_box[2] and crop_box[1] < crop_box[3]:
            cropped_img = img.crop(crop_box)

            if overwrite:
                save_path = filename
            else:
                save_path = os.path.join(output_dir, filename)

            cropped_img.save(save_path)
            print(f"Cropped {filename} -> {save_path}")
        else:
            print(f"⚠️ Skipped {filename}: invalid crop dimensions")
