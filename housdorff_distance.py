import pymeshlab

ms = pymeshlab.MeshSet()

ms.load_new_mesh('Debug/Samples/displaced/pallas_125_to_5000_disp_100_edge_0.7.obj')
ms.load_new_mesh('Debug/Samples/pallas_5000.obj')
dist = ms.get_hausdorff_distance()
print(dist)
