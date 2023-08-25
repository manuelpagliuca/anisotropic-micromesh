import os
import argparse
import subprocess
import glob
import re
import pymeshlab


ms = pymeshlab.MeshSet()
ms.load_new_mesh(".\Models\orc_bust_75000.obj")
ms.load_new_mesh(".\Output\Evaluation\same_microfaces\micro\orc_bust_146\\to_75000_faces_11984.obj")
ms.load_new_mesh(".\Output\Evaluation\same_microfaces\micro\orc_bust_146\\to_75000_faces_8048.obj")
ms.load_new_mesh(".\Output\Evaluation\same_microfaces\micro\orc_bust_146\\to_75000_faces_4232.obj")
ms.load_new_mesh(".\Output\Evaluation\same_microfaces\micro\orc_bust_146\\to_75000_faces_3524.obj")

print(ms.get_hausdorff_distance(sampledmesh = 0, targetmesh = 1, samplenum = 4066))
print(ms.get_hausdorff_distance(sampledmesh = 0, targetmesh = 2, samplenum = 4066))
print(ms.get_hausdorff_distance(sampledmesh = 0, targetmesh = 3, samplenum = 4066))
print(ms.get_hausdorff_distance(sampledmesh = 0, targetmesh = 4, samplenum = 4066))