import os
import argparse
import subprocess
import glob
import re
import pymeshlab
from decimal import Decimal, ROUND_HALF_UP


def export_hausdorff(base_mesh_name, target_mesh_path, displaced_samples_path):
    # Get the list of files
    file_list = glob.glob(os.path.join(displaced_samples_path, "*"))
    # Consider only OBJ and OFF
    displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]
    # Sort the meshes by dimension (amount of micro-faces)
    displaced_meshes.sort(key=os.path.getsize)
    # Initialise the factor (will be written in the .txt)
    factor = Decimal('1.0')

    scheme = displaced_samples_path.split("/")[2]

    # R/W a .txt file
    with open(f"{displaced_samples_path}/hausdorff_{base_mesh_name}_{scheme}.txt", "w") as output_file:

        # Write the header
        output_file.write("microFaces factor RMS max mean RMS_scaled max_scaled mean_scaled\n")

        for displaced_mesh_path in displaced_meshes:
            # Load the displaced and target mesh in a MeshSet
            ms = pymeshlab.MeshSet()
            ms.load_new_mesh(displaced_mesh_path)
            ms.load_new_mesh(target_mesh_path)

            # Extract the number of micro-faces (expressed in millions, e.g. 1.3mln)
            target_mesh_name = os.path.basename(displaced_mesh_path)
            mfs = int(target_mesh_name.split("_")[3])
            mln_mfs = Decimal(mfs) / Decimal(1000000)
            mln_mfs = mln_mfs.quantize(Decimal('0.00'), rounding=ROUND_HALF_UP)

            # Compute Hausdorff's distances
            res = ms.get_hausdorff_distance(sampledmesh=0, targetmesh=1)

            # Compute the diagonal of the BBOX for scale the absolute distances
            diagonal = ms.current_mesh().bounding_box().diagonal()

            # Scale the distances
            scaled_RMS = (res['RMS'] * 100) / diagonal
            scaled_max = (res['max'] * 100) / diagonal
            scaled_mean = (res['mean'] * 100) / diagonal

            # Write micro-faces, factor and distances in a row
            output_file.write(str(mln_mfs) + " ")
            output_file.write(f'{factor:.2f} ')
            output_file.write(str(res['RMS']) + " ")
            output_file.write(str(res['max']) + " ")
            output_file.write(str(res['mean']) + " ")
            output_file.write(str(scaled_RMS) + " ")
            output_file.write(str(scaled_max) + " ")
            output_file.write(str(scaled_mean) + " ")

            output_file.write(" \\\\ \n")

            # Increase the factor by 0.1
            factor = factor + Decimal('0.1')
    print(
        f"Hausdorff's distances saved > {displaced_samples_path}/hausdorff_{base_mesh_name}_{scheme}.txt")
    output_file.close()


def generate_samples(params):
    exe_path = os.path.join(os.getcwd(), "Release", "anisotropic_micromesh.exe")

    MIN_MFS_FACTOR = 1.0
    MAX_MFS_FACTOR = 4.1
    STEP_SIZE = 0.1

    # Using the received params, run 'gen-sample' cmd 30 times, from 1.0 to 4.0
    for num in range(int(MIN_MFS_FACTOR * 10), int(MAX_MFS_FACTOR * 10), int(STEP_SIZE * 10)):
        factor = num / 10.0
        params.append(f"--factor={factor}")

        try:
            subprocess.check_call([exe_path, "gen-sample"] + params)
        except subprocess.CalledProcessError as e:
            print(f"Error while executing the command: {e}")
        except FileNotFoundError as e:
            print(f"File not found: {e}")

        params.pop()


if __name__ == "__main__":
    # Parsing command-line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("--base-mesh", default="pallas_1000.obj")
    parser.add_argument("--target-mesh", default="original_pallas_triquad.obj")
    args = parser.parse_args()

    # Building command-line arguments for generating the samples
    params = []

    if args.base_mesh is not None:
        params.append(f"--base-mesh={args.base_mesh}")

    if args.target_mesh is not None:
        params.append(f"--target-mesh={args.target_mesh}")

    # Generating samples for both schemes
    # params.append("--scheme=micro")
    # generate_samples(params)
    # params.pop()
    # params.append("--scheme=aniso")
    # generate_samples(params)

    # Exporting Hausdorff distances
    base_mesh_name = args.base_mesh[:-4]
    iso_samples_path = "./Evaluation/micro/" + base_mesh_name
    aniso_samples_path = "./Evaluation/aniso/" + base_mesh_name
    target_mesh_path = "./Models/" + args.target_mesh

    export_hausdorff(base_mesh_name, target_mesh_path, iso_samples_path)
    export_hausdorff(base_mesh_name, target_mesh_path, aniso_samples_path)
