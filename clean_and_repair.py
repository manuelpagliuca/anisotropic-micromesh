import os
import argparse
import subprocess
import glob
import re
import pymeshlab


models_dir = "Models"
mesh_list = glob.glob(os.path.join(models_dir, "*"))

for mesh in mesh_list:
    ms = pymeshlab.MeshSet()
    ms.load_new_mesh(mesh)
    ms.meshing_repair_non_manifold_edges()
    ms.meshing_repair_non_manifold_vertices()
    ms.meshing_remove_null_faces()
    ms.meshing_remove_unreferenced_vertices()
    ms.meshing_remove_connected_component_by_diameter()
    ms.meshing_remove_folded_faces()
    ms.meshing_remove_duplicate_vertices()
    ms.meshing_remove_duplicate_faces()

    outputPath = models_dir + mesh[len(models_dir):]
    ms.save_current_mesh(outputPath, save_face_color=False)
    print(outputPath)
