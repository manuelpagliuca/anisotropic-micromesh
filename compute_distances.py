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

      for key, value in ms.get_hausdorff_distance().items():
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

      for key, value in ms.get_hausdorff_distance().items():
        output_file.write(str(value) + " ")

      output_file.write("\n")
  output_file.close()
  print(f"Hausdorff's distances > {outputDir}/hausdorff_microfaces_aniso.txt")


def hausdorff_same_target_edges(base_mesh_name, metric_name, target_mesh_path, dir_path_micro, dir_path_aniso):
  file_list = glob.glob(os.path.join(dir_path_micro, "*"))
  displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]

  with open(f"./Output/Evaluation/{metric_name}/micro/hausdorff_{base_mesh_name}.txt", "w") as output_file:
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
      f"Hausdorff's distances > ./Output/Evaluation/{metric_name}/micro/hausdorff_{base_mesh_name}.txt")

  file_list = glob.glob(os.path.join(dir_path_aniso, "*"))
  displaced_meshes = [file for file in file_list if os.path.splitext(file)[1] in ['.obj', '.off']]

  with open(f"./Output/Evaluation/{metric_name}/aniso/hausdorff_{base_mesh_name}.txt", "w") as output_file:
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
      f"Hausdorff's distances > ./Output/Evaluation/{metric_name}/aniso/hausdorff_{base_mesh_name}.txt")

  output_file.close()


# Parse args
parser = argparse.ArgumentParser(description="Description of your application.")
parser.add_argument("--base-mesh", default="pallas_5000.obj", help="Parameter 5 (base-mesh)")
parser.add_argument("--target-mesh", default="pallas_124.obj", help="Parameter 6 (target)")
parser.add_argument("--metric", default="same-target-edges", help="Parameter 7 (metric)")
parser.add_argument("--clean", help="Parameter 8 (clean flag)", action="store_true")
args = parser.parse_args()

# Setup paths
metric_name = args.metric.replace("-", "_")
dir_path_micro = f"./Output/Evaluation/{metric_name}/micro"
dir_path_aniso = f"./Output/Evaluation/{metric_name}/aniso"
target_mesh_path = "./Models/" + args.target_mesh
target_mesh_faces = extract_last_integer_number_from_string(args.target_mesh[:-4])
exe_path = os.path.join(os.getcwd(), "Debug", "master_thesis.exe")  # Should be changed to Release

if args.clean:
  delete_files_in_directory(dir_path_micro)
  delete_files_in_directory(dir_path_aniso)

if args.metric == "same-microfaces":
  hausdorff_same_microfaces(args.base_mesh[:-4], target_mesh_faces,
                            target_mesh_path, dir_path_micro, dir_path_aniso)
elif args.metric == "same-target-edges":
  hausdorff_same_target_edges(args.base_mesh[:-4], metric_name,
                              target_mesh_path, dir_path_micro, dir_path_aniso)
