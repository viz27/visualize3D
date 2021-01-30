# Scalar Field Visualization
An OPENGL tool for scalar field visualization. The tool can be run in two modes.
<br> FAST Mode: Where all grid cells for the specified scalar values will be precomputed for faster contor extraction.
<br> SEED Mode: Where seed set will be computed and saved in 1st run and this will be read for contour propagation in subsequent runs.

Detailed description can be found in Report.pdf file.

# Config File
Each data set file needs a config file of the same name
Config file should contains 3 lines
<br>1 - spacing(eg.1x1x1)
<br>2 - default iso value to display
<br>3 - extra iso values for which seeds are to be stored. First an integer denoting number of extra values(say n) followed by n space separated float values.

# Building
Install OpenGL and other supporting libraries by following the commands in opengl_installation_instructions.txt. 

Run ```make``` to build the tool.

# Run Command
Example command to run in FAST Mode.
```
./visualize Data/teapot_256x256x178_uint8
```

Example command to run in SEED Mode.
```
./visualize Data/teapot_256x256x178_uint8 seed_mode
```
