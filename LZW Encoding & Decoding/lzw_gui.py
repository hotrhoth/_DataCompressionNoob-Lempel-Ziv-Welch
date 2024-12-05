import ctypes
import tkinter as tk
from tkinter import filedialog, messagebox

# Load the shared library (Make sure the path to the .so or .dll file is correct)
lzw_lib = ctypes.CDLL('./liblzw.so')

# Define the argument types for the compress and decompress functions
lzw_lib.compress.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lzw_lib.decompress.argtypes = [ctypes.c_char_p, ctypes.c_char_p]

# Variables to store file paths
input_file_path = ""
output_file_path = ""

def browse_file():
    global input_file_path
    file_path = filedialog.askopenfilename(title="Open File", filetypes=[("Binary files", "*.bin"), ("Text files", "*.txt")])
    if file_path:
        input_file_path = file_path
        messagebox.showinfo("Input File Selected", f"Input file selected")

def browse_output():
    global output_file_path
    file_path = filedialog.asksaveasfilename(title="Save File", filetypes=[("Binary files", "*.bin"), ("Text files", "*.txt")])
    if file_path:
        output_file_path = file_path 
        messagebox.showinfo("Output File Selected", f"Output file selected")

def compress_file():
    global input_file_path, output_file_path

    if not input_file_path or not output_file_path:
        messagebox.showerror("Error", "Please select input and output files.")
        return

    try:
        lzw_lib.compress(input_file_path.encode('utf-8'), output_file_path.encode('utf-8'))
        messagebox.showinfo("Success", "File compressed successfully!")
    except Exception as e:
        messagebox.showerror("Error", f"Error during compression: {str(e)}")

def decompress_file():
    global input_file_path, output_file_path

    if not input_file_path or not output_file_path:
        messagebox.showerror("Error", "Please select input and output files.")
        return

    try:
        # Call the C++ decompress function
        lzw_lib.decompress(input_file_path.encode('utf-8'), output_file_path.encode('utf-8'))
        messagebox.showinfo("Success", "File decompressed successfully!")
    except Exception as e:
        messagebox.showerror("Error", f"Error during decompression: {str(e)}")

# Create the GUI window
window = tk.Tk()
window.title("LZW Algorithm Demo")

# Row for selecting the input file and action buttons
tk.Button(window, text="Browse Input File", command=browse_file).grid(row=0, column=0, padx=5, pady=5)
tk.Button(window, text="Compress", command=compress_file).grid(row=0, column=1, padx=5, pady=5)

# Row for selecting the output file
tk.Button(window, text="Browse Output File", command=browse_output).grid(row=1, column=0, padx=5, pady=5)
tk.Button(window, text="Decompress", command=decompress_file).grid(row=1, column=1, padx=5, pady=5)

window.mainloop()
