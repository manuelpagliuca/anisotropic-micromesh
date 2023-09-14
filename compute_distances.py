import os
import argparse
import subprocess
import glob
import re
import pymeshlab


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


def hausdorff_same_microfaces(base_mesh_name, target_mesh_faces, target_mesh_path, dir_path_micro, dir_path_aniso):
    dir_list = glob.glob(os.path.join(dir_path_micro, "*"))
    outputDir = ""

    for mesh_dir in dir_list:
        if base_mesh_name in mesh_dir:
            outputDir = mesh_dir

    file_list = glob.glob(os.path.join(outputDir, "*"))
    displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]

    mfs_factor = 1.0

    with open(f"{outputDir}/hausdorff_{base_mesh_name}_to_{target_mesh_faces}.txt", "w") as output_file:
        output_file.write("mfsFactor RMS max mean min\n")

        for displaced_mesh_path in displaced_meshes:
            ms = pymeshlab.MeshSet()
            ms.load_new_mesh(target_mesh_path)
            ms.load_new_mesh(displaced_mesh_path)
            output_file.write(mfs_factor)
            mfs_factor = mfs_factor + 0.1
            distances = ms.get_hausdorff_distance(sampledmesh=0, targetmesh=1)

            for key, value in distances.items():
                if key == "n_samples" or key == "diag_mesh_0" or key == "diag_mesh_1" or key == "min":
                    continue
                if isinstance(value, (int, float)):
                    value = round(value, 4)
                output_file.write(" & " + str(value))
            output_file.write("\\\\ \n")
            print("Computed distances for: " + displaced_mesh_path)
    output_file.close()

    print(f"Hausdorff's distances > {outputDir}/hausdorff_microfaces_micro.txt")

    dir_list = glob.glob(os.path.join(dir_path_aniso, "*"))

    for mesh_dir in dir_list:
        if base_mesh_name in mesh_dir:
            outputDir = mesh_dir

    file_list = glob.glob(os.path.join(outputDir, "*"))

    displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]

    mfs_factor = 1.0

    with open(f"{outputDir}/hausdorff_{base_mesh_name}_to_{target_mesh_faces}.txt", "w") as output_file:
        output_file.write("microFaces RMS max mean min\n")

        for displaced_mesh_path in displaced_meshes:
            ms = pymeshlab.MeshSet()
            ms.load_new_mesh(target_mesh_path)
            ms.load_new_mesh(displaced_mesh_path)

            output_file.write(mfs_factor)
            mfs_factor = mfs_factor + 0.1

            distances = ms.get_hausdorff_distance(sampledmesh=0, targetmesh=1)

            for key, value in distances.items():
                if key == "n_samples" or key == "diag_mesh_0" or key == "diag_mesh_1" or key == "min":
                    continue
                if isinstance(value, (int, float)):
                    value = round(value, 4)
                output_file.write(" & " + str(value))
            output_file.write("\\\\ \n")
            print("Computed distances for: " + displaced_mesh_path)
    output_file.close()

    print(f"Hausdorff's distances > {outputDir}/hausdorff_microfaces_aniso.txt")


def generating_sample():
    exe_path = os.path.join(os.getcwd(), "Debug", "anisotropic_micromesh.exe")

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


# Parse args
parser = argparse.ArgumentParser(
    description="Generating needed samples and computing hausdorff distances.")

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

params.append("--scheme=micro")
generating_sample()
params.pop()  # popping the isotropic scheme
params.append("--scheme=aniso")
generating_sample()

# computing hausdorff distances

base_mesh_name = args.base_mesh[:-4]
iso_samples_path = f"./Evaluation/same_microfaces/micro/" + base_mesh_name
aniso_samples_path = f"./Evaluation/same_microfaces/aniso/" + base_mesh_name
target_mesh_path = "./Models/" + args.target_mesh
target_mesh_faces = extract_last_integer_number_from_string(args.target_mesh[:-4])

hausdorff_same_microfaces(
    base_mesh_name, target_mesh_faces,
    target_mesh_path, iso_samples_path, aniso_samples_path)
