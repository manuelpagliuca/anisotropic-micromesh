import os
import argparse
import subprocess
import glob
import re
import pymeshlab
from decimal import Decimal, ROUND_HALF_UP


def clear_dir(directory_path):
    file_list = os.listdir(directory_path)
    for file_name in file_list:
        file_path = os.path.join(directory_path, file_name)
        try:
            if os.path.isfile(file_path):
                os.remove(file_path)
                print(f"File '{file_name}' successfully deleted.")
            else:
                print(f"Unable to delete '{file_name}' as it is not a file.")
        except Exception as e:
            print(f"Error while deleting '{file_name}': {str(e)}")


def extract_last_integer_number_from_string(input_string):
    integer_numbers = re.findall(r"\d+", input_string)

    if integer_numbers:
        return int(integer_numbers[-1])
    else:
        return None


def hausdorff_same_microfaces(base_mesh_name, target_mesh_faces, target_mesh_path, iso_samples_path, aniso_samples_path):
    file_list = glob.glob(os.path.join(iso_samples_path, "*"))
    displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]
    displaced_meshes.sort(key=os.path.getsize)

    with open(f"{iso_samples_path}/hausdorff_{base_mesh_name}_micro.txt", "w") as output_file:
        output_file.write("microFaces RMS max mean\n")

        for displaced_mesh_path in displaced_meshes:
            ms = pymeshlab.MeshSet()
            ms.load_new_mesh(displaced_mesh_path)
            ms.load_new_mesh(target_mesh_path)

            target_mesh_name = os.path.basename(displaced_mesh_path)
            mfs = int(target_mesh_name.split("_")[3])
            mln_mfs = Decimal(mfs) / Decimal(1000000)
            mln_mfs = mln_mfs.quantize(Decimal('0.00'), rounding=ROUND_HALF_UP)

            res = ms.get_hausdorff_distance(sampledmesh=0, targetmesh=1)

            output_file.write(str(mln_mfs))
            output_file.write(str(res['RMS']) + " ")
            output_file.write(str(res['max']) + " ")
            output_file.write(str(res['mean']) + " ")
            output_file.write(" \\\\ \n")

            print("Computed H.D. for: " + displaced_mesh_path)

    output_file.close()

    print(f"Hausdorff's distances > {iso_samples_path}/hausdorff_{base_mesh_name}_micro.txt")

    file_list = glob.glob(os.path.join(aniso_samples_path, "*"))
    displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]
    displaced_meshes.sort(key=os.path.getsize)

    with open(f"{aniso_samples_path}/hausdorff_{base_mesh_name}_aniso.txt", "w") as output_file:
        output_file.write("microFaces RMS max mean\n")

        for displaced_mesh_path in displaced_meshes:
            ms = pymeshlab.MeshSet()
            ms.load_new_mesh(displaced_mesh_path)
            ms.load_new_mesh(target_mesh_path)

            target_mesh_name = os.path.basename(displaced_mesh_path)
            mfs = int(target_mesh_name.split("_")[3])
            mln_mfs = Decimal(mfs) / Decimal(1000000)
            mln_mfs = mln_mfs.quantize(Decimal('0.00'), rounding=ROUND_HALF_UP)

            res = ms.get_hausdorff_distance(sampledmesh=0, targetmesh=1)

            output_file.write(str(mln_mfs))
            output_file.write(str(res['RMS']) + " ")
            output_file.write(str(res['max']) + " ")
            output_file.write(str(res['mean']) + " ")
            output_file.write(" \\\\ \n")

            print("Computed H.D. for: " + displaced_mesh_path)
    output_file.close()

    print(f"Hausdorff's distances > {aniso_samples_path}/hausdorff_{base_mesh_name}_aniso.txt")


def generating_sample():
    exe_path = os.path.join(os.getcwd(), "Release", "anisotropic_micromesh.exe")

    MIN_MFS_FACTOR = 1.0
    MAX_MFS_FACTOR = 4.1
    STEP_SIZE = 0.1

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


parser = argparse.ArgumentParser()
parser.add_argument("--base-mesh", default="pallas_1000.obj", help="Parameter 3 (base-mesh)")
parser.add_argument("--target-mesh", default="original_pallas_triquad.obj",
                    help="Parameter 4 (target)")
parser.add_argument("--clean", help="Parameter 6 (clean flag)", action="store_true")

args = parser.parse_args()

if args.clean:
    clear_dir(dir_path_micro)
    clear_dir(dir_path_aniso)

params = []

if args.base_mesh is not None:
    params.append(f"--base-mesh={args.base_mesh}")

if args.target_mesh is not None:
    params.append(f"--target-mesh={args.target_mesh}")

# params.append("--scheme=micro")
# generating_sample()
# params.pop()
# params.append("--scheme=aniso")
# generating_sample()

# computing hausdorff distances

base_mesh_name = args.base_mesh[:-4]
iso_samples_path = f"./Evaluation/micro/" + base_mesh_name
aniso_samples_path = f"./Evaluation/aniso/" + base_mesh_name
target_mesh_path = "./Models/" + args.target_mesh
target_mesh_faces = extract_last_integer_number_from_string(args.target_mesh[:-4])

hausdorff_same_microfaces(
    base_mesh_name, target_mesh_faces,
    target_mesh_path, iso_samples_path, aniso_samples_path)
