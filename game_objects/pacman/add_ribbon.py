from PIL import Image
import os

# === COLOR CONFIGURATION ===
original_color = (255, 255, 0)           # yellow
pink_color = (255, 192, 203)             # light pink
light_blue_color = (173, 216, 230)       # light blue

# === Helper: fuzzy color matcher ===
def close_enough(c1, c2, tolerance=40):
    return all(abs(a - b) < tolerance for a, b in zip(c1, c2))

# === Recolor function ===
def recolor_folder(folder, new_color):
    print(f"🎨 Recoloring {folder} sprites...")
    for filename in os.listdir(folder):
        if not filename.lower().endswith(".png"):
            continue

        path = os.path.join(folder, filename)
        sprite = Image.open(path).convert("RGBA")
        pixels = sprite.getdata()

        new_pixels = []
        for pixel in pixels:
            r, g, b, a = pixel
            if close_enough((r, g, b), original_color):
                new_pixels.append(new_color + (a,))
            else:
                new_pixels.append((r, g, b, a))

        sprite.putdata(new_pixels)
        sprite.save(path)

# === Run on both folders ===
recolor_folder("red", pink_color)
recolor_folder("blue", light_blue_color)

print("✅ Done! Red sprites are pink, blue sprites are light blue.")
