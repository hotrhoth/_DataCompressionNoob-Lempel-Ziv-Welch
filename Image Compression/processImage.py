import os
import numpy as np
import tkinter as tk
from tkinter import filedialog
from PIL import Image, ImageTk

def processImage(input_path, output_path):
    image = Image.open(input_path).convert("RGB")

    shape = np.array(image).shape
    image_array = np.array(image).flatten()

    with open(output_path, "w") as f:
        f.write(f"{shape[0]},{shape[1]},{shape[2]}\n")
        f.write(",".join(map(str, image_array)))
    
    print(f"Image saved as NumPy data to {output_path}")

def readDecompressedFile(file_path):
    with open(file_path, 'r') as file:
        shape_line = file.readline().strip()
        data_line = file.readline().strip()

    height, width, channels = map(int, shape_line.split(','))
    pixels = list(map(int, data_line.split(',')))

    return height, width, channels, pixels

def saveTif(height, width, channels, pixels, output_file):
    image_array = np.array(pixels, dtype=np.uint8).reshape((height, width, channels))

    image = Image.fromarray(image_array, mode='RGB')
    image.save(output_file)

    print(f"Decompressed image saved as {output_file}")

def open_file1():
    """
    Open a .tif file for the first display area.
    """
    file_path = filedialog.askopenfilename(
        filetypes=[("TIFF files", "*.tif"), ("All files", "*.*")]
    )
    if file_path:
        display_image(file_path, image_label1)

def open_file2():
    """
    Open a .tif file for the second display area.
    """
    file_path = filedialog.askopenfilename(
        filetypes=[("TIFF files", "*.tif"), ("All files", "*.*")]
    )
    if file_path:
        display_image(file_path, image_label2)

def display_image(file_path, image_label):
    """
    Load and display the image in the specified label.
    """
    try:
        image = Image.open(file_path)
        image.thumbnail((200, 200))  # Resize to fit a 200x200 box

        # Convert image to a format Tkinter can use
        img_tk = ImageTk.PhotoImage(image)

        # Update the label to show the image
        image_label.config(image=img_tk)
        image_label.image = img_tk  # Keep a reference to avoid garbage collection

    except Exception as e:
        print(f"Error displaying image: {e}")
    
if __name__ == "__main__":

    # # Process Image to Numpy data saved in .txt
    # input_path1 = "leaves.tif"
    # output_path1 = "leaves.txt"
    # processImage(input_path1, output_path1)

    # ------------------------------------------------------------------------------------------------------

    # # Decompressed Numpy data to picture saved in .tif
    # input_path2 = "decompressed_leaves.txt"
    # output_path2 = "decompressed_leaves.tif"

    # height, width, channels, pixels = readDecompressedFile(input_path2)
    # saveTif(height, width, channels, pixels, output_path2)

    # ------------------------------------------------------------------------------------------------------

    # Display 2 .tif files
    root = tk.Tk()
    root.title("TIFF Viewer - Two Images")
    root.geometry("450x250")

    # Create frames for better layout
    frame = tk.Frame(root)
    frame.pack(pady=10, fill=tk.BOTH, expand=True)

    # Add buttons to load the first and second images
    button_frame = tk.Frame(root)
    button_frame.pack(pady=10)

    open_button1 = tk.Button(button_frame, text="Open First .tif File", command=open_file1)
    open_button1.grid(row=0, column=0, padx=10)

    open_button2 = tk.Button(button_frame, text="Open Second .tif File", command=open_file2)
    open_button2.grid(row=0, column=1, padx=10)

    # Create labels to display the images side by side
    image_label1 = tk.Label(frame)
    image_label1.grid(row=0, column=0, padx=10)

    image_label2 = tk.Label(frame)
    image_label2.grid(row=0, column=1, padx=10)

    # Run the application
    root.mainloop()





