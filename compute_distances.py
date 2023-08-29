import os
import argparse
import subprocess
import glob
import re
import pymeshlab


def delete_files_in_directory(directory_path):
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


def execute_command(command):
  try:
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    if result.returncode == 0:
      print("Command executed successfully!")
      print("Output:")
      print(result.stdout)
    else:
      print(f"Command failed with exit code: {result.returncode}")
      print("Error:")
      print(result.stderr)
  except Exception as e:
    print(f"Error while executing the command: {str(e)}")


def extract_second_number_from_string(input_string):
  numbers = re.findall(r"\d+", input_string)

  if len(numbers) >= 2:
    return int(numbers[1])
  else:
    return None


def extract_last_decimal_number_from_string(input_string):
  decimal_numbers = re.findall(r"\d+\.\d+", input_string)

  if decimal_numbers:
    return float(decimal_numbers[-1])
  else:
    return None


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

  with open(f"{outputDir}/hausdorff_{base_mesh_name}_to_{target_mesh_faces}.txt", "w") as output_file:
    output_file.write("microFaces RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

    for displaced_mesh_path in displaced_meshes:
      ms = pymeshlab.MeshSet()
      ms.load_new_mesh(target_mesh_path)
      ms.load_new_mesh(displaced_mesh_path)
      output_file.write(str(extract_last_integer_number_from_string(displaced_mesh_path)) + " ")
      distances = ms.get_hausdorff_distance(sampledmesh=0, targetmesh=1, samplenum=4066)

      for key, value in distances.items():
        output_file.write(str(value) + " ")

      output_file.write("\n")
  output_file.close()

  print(f"Hausdorff's distances > {outputDir}/hausdorff_microfaces_micro.txt")

  dir_list = glob.glob(os.path.join(dir_path_aniso, "*"))

  for file in dir_list:
    if base_mesh_name in file:
      outputDir = file

  file_list = glob.glob(os.path.join(outputDir, "*"))
  displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]

  with open(f"{outputDir}/hausdorff_{base_mesh_name}_to_{target_mesh_faces}.txt", "w") as output_file:
    output_file.write("microFaces RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

    for displaced_mesh_path in displaced_meshes:
      ms = pymeshlab.MeshSet()
      ms.load_new_mesh(target_mesh_path)
      ms.load_new_mesh(displaced_mesh_path)
      output_file.write(str(extract_last_integer_number_from_string(displaced_mesh_path)) + " ")

      for key, value in ms.get_hausdorff_distance(sampledmesh=0, targetmesh=1, samplenum=4066).items():
        output_file.write(str(value) + " ")

      output_file.write("\n")
  output_file.close()
  print(f"Hausdorff's distances > {outputDir}/hausdorff_microfaces_aniso.txt")

  file_list = glob.glob(os.path.join(dir_path_micro, "*"))
  displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]

  with open(f"./Output/Evaluation/{criterion_name}/micro/hausdorff_{base_mesh_name}.txt", "w") as output_file:
    output_file.write("edgeLength RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

    for displaced_mesh_path in displaced_meshes:
      ms = pymeshlab.MeshSet()
      ms.load_new_mesh(target_mesh_path)
      ms.load_new_mesh(displaced_mesh_path)

      output_file.write(str(extract_last_decimal_number_from_string(displaced_mesh_path)) + " ")

      for key, value in ms.get_hausdorff_distance().items():
        output_file.write(str(value) + " ")

      output_file.write("\n")

  print(
      f"Hausdorff's distances > ./Output/Evaluation/{criterion_name}/micro/hausdorff_{base_mesh_name}.txt")

  file_list = glob.glob(os.path.join(dir_path_aniso, "*"))
  displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]

  with open(f"./Output/Evaluation/{criterion_name}/aniso/hausdorff_{base_mesh_name}.txt", "w") as output_file:
    output_file.write("edgeLength RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

    for displaced_mesh_path in displaced_meshes:
      ms = pymeshlab.MeshSet()
      ms.load_new_mesh(target_mesh_path)
      ms.load_new_mesh(displaced_mesh_path)

      output_file.write(str(extract_last_decimal_number_from_string(displaced_mesh_path)) + " ")

      for key, value in ms.get_hausdorff_distance().items():
        output_file.write(str(value) + " ")

      output_file.write("\n")
  print(
      f"Hausdorff's distances > ./Output/Evaluation/{criterion_name}/aniso/hausdorff_{base_mesh_name}.txt")

  output_file.close()


# Parse args
parser = argparse.ArgumentParser(description="Description of your application.")
parser.add_argument("--min-edge", type=float, help="Parameter 1 (min-edge)")
parser.add_argument("--max-edge", type=float, help="Parameter 2 (max-edge)")
parser.add_argument("--base-mesh", default="pallas_5000.obj", help="Parameter 3 (base-mesh)")
parser.add_argument("--target-mesh", default="pallas_124.obj", help="Parameter 4 (target)")
parser.add_argument("--criterion", default="same-target-edges", help="Parameter 5 (criterion)")
parser.add_argument("--clean", help="Parameter 6 (clean flag)", action="store_true")
args = parser.parse_args()

# Setup paths
criterion_name = args.criterion.replace("-", "_")
dir_path_micro = f"./Output/Evaluation/same_microfaces/micro"
dir_path_aniso = f"./Output/Evaluation/same_microfaces/aniso"
target_mesh_path = "./Models/" + args.target_mesh
target_mesh_faces = extract_last_integer_number_from_string(args.target_mesh[:-4])
# Should be changed to Release
exe_path = os.path.join(os.getcwd(), "Debug", "anisotropic_micromesh.exe")

if args.clean:
  delete_files_in_directory(dir_path_micro)
  delete_files_in_directory(dir_path_aniso)

params = []

if args.min_edge is not None:
  params.append(f"--min-edge={args.min_edge}")

if args.max_edge is not None:
  params.append(f"--max-edge={args.max_edge}")

if args.base_mesh is not None:
  params.append(f"--base-mesh={args.base_mesh}")

if args.target_mesh is not None:
  params.append(f"--target-mesh={args.target_mesh}")

params.append("--scheme=micro")

for i in range(1000, 110000, 1000):
  params.append(f"--microfaces={i}")
  try:
    print([exe_path, "gen-sample"] + params)
    subprocess.check_call([exe_path, "gen-sample"] + params)
  except subprocess.CalledProcessError as e:
    print(f"Error while executing the command: {e}")
  except FileNotFoundError as e:
    print(f"File not found: {e}")

params.pop()
params.append("--scheme=aniso")

for i in range(1000, 110000, 1000):
  params.append(f"--microfaces={i}")
  try:
    print([exe_path, "gen-sample"] + params)
    subprocess.check_call([exe_path, "gen-sample"] + params)
  except subprocess.CalledProcessError as e:
    print(f"Error while executing the command: {e}")
  except FileNotFoundError as e:
    print(f"File not found: {e}")

hausdorff_same_microfaces(args.base_mesh[:-4], target_mesh_faces,
                          target_mesh_path, dir_path_micro, dir_path_aniso)
