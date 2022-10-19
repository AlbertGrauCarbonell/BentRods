# BentRods

The files concerning this repository were used to generate a a 3D matrix of voxels that represent a ground truth dataset of bent rods whose fluorescence is on their outer layer. The way this process works is by extracting the coordinates for such bent rods from a file generated elsewhere, and then determining which pixels from the voxel matrix correspond to the outer layers of such rods and giving them a signal value. In our case, non-bright voxels have a value of 0, whereas a bright pixel has a value of 1.

The file with the rod coordinates is: Configuration1.dat
The file with the main code (determining which voxels are these of then outer shell of the bent rods) is: overlaympxfull.c
The file sued to make the executable for this code is: Makefile

