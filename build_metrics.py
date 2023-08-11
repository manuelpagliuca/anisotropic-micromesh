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


# Parse the args
parser = argparse.ArgumentParser(description="Description of your application.")
parser.add_argument("-n", type=int, help="Parameter 2 (n)")
parser.add_argument("--min-edge", type=float, help="Parameter 3 (min-edge)")
parser.add_argument("--max-edge", type=float, help="Parameter 4 (max-edge)")
parser.add_argument("--base-mesh", help="Parameter 5 (base-mesh)")
parser.add_argument("--target-mesh", help="Parameter 6 (target)")
parser.add_argument("--metric", default="same-target-edges", help="Parameter 7 (metric)")
parser.add_argument("--clean", help="Parameter 8 (clean flag)", action="store_true")

args = parser.parse_args()

# Setup paths
ev_metric_dir = args.metric.replace("-", "_")
directory_path_micro = f"./Output/Evaluation/{ev_metric_dir}/micro"
directory_path_aniso = f"./Output/Evaluation/{ev_metric_dir}/aniso"
target_mesh_path = "./Models/" + (args.target_mesh if args.target_mesh else "pallas_5000.obj")

exe_path = os.path.join(os.getcwd(), "Debug", "master_thesis.exe")

# Clean the samples dirs
if args.clean:
  delete_files_in_directory(directory_path_micro)
  delete_files_in_directory(directory_path_aniso)

params = []

if args.n is not None:
  params.append(f"-n={args.n}")

if args.min_edge is not None:
  params.append(f"--min-edge={args.min_edge}")

if args.max_edge is not None:
  params.append(f"--max-edge={args.max_edge}")

if args.base_mesh is not None:
  params.append(f"--base-mesh={args.base_mesh}")

if args.target_mesh is not None:
  params.append(f"--target-mesh={args.target_mesh}")

if args.metric is not None:
  params.append(f"--metric={args.metric}")

# Setup generic variables
if args.target_mesh:
  target_n_faces = extract_last_integer_number_from_string(args.target_mesh[:-4])

# Execute command
try:
  subprocess.check_call([exe_path, "build-samples"] + params)
except subprocess.CalledProcessError as e:
  print(f"Error while executing the command: {e}")
except FileNotFoundError as e:
  print(f"File not found: {e}")


# Switch data analysis in base of selected metric
if args.metric == "same-target-edges":
  file_list = glob.glob(os.path.join(directory_path_micro, "*"))

  with open(f"./Output/Evaluation/{ev_metric_dir}/hausdorff_micro.txt", "w") as output_file:
    output_file.write("edgeLength RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

    for displaced_mesh_path in file_list:
      ms = pymeshlab.MeshSet()
      ms.load_new_mesh(target_mesh_path)
      ms.load_new_mesh(displaced_mesh_path)
      output_file.write(str(extract_last_decimal_number_from_string(displaced_mesh_path)) + " ")

      for key, value in ms.get_hausdorff_distance().items():
        output_file.write(str(value) + " ")

      output_file.write("\n")

  print(f"Hausdorff's distances > ./Output/Evaluation/{ev_metric_dir}/hausdorff_micro.txt")

  file_list = glob.glob(os.path.join(directory_path_aniso, "*"))

  with open(f"./Output/Evaluation/{ev_metric_dir}/hausdorff_aniso.txt", "w") as output_file:
    output_file.write("edgeLength RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

    for displaced_mesh_path in file_list:
      ms = pymeshlab.MeshSet()
      ms.load_new_mesh(target_mesh_path)
      ms.load_new_mesh(displaced_mesh_path)
      output_file.write(str(extract_last_decimal_number_from_string(displaced_mesh_path)) + " ")

      for key, value in ms.get_hausdorff_distance().items():
        output_file.write(str(value) + " ")

      output_file.write("\n")
  print(f"Hausdorff's distances > ./Output/Evaluation/{ev_metric_dir}/hausdorff_aniso.txt")

  output_file.close()

elif args.metric == "same-microfaces":
  dir_list = glob.glob(os.path.join(directory_path_micro, "*"))
  outputDir = ""

  for file in dir_list:
    if args.base_mesh[:-4] in file:
      outputDir = file

  print(outputDir)
  file_list = glob.glob(os.path.join(outputDir, "*"))

  with open(f"{outputDir}/hausdorff_{args.base_mesh[:-4]}_to_{target_n_faces}.txt", "w") as output_file:
    output_file.write("microFaces RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

    for displaced_mesh_path in file_list:
      ms = pymeshlab.MeshSet()
      ms.load_new_mesh(target_mesh_path)
      ms.load_new_mesh(displaced_mesh_path)
      output_file.write(str(extract_last_integer_number_from_string(displaced_mesh_path)) + " ")

      for key, value in ms.get_hausdorff_distance().items():
        output_file.write(str(value) + " ")

      output_file.write("\n")
  output_file.close()

  print(f"Hausdorff's distances > {outputDir}/hausdorff_microfaces_micro.txt")

  dir_list = glob.glob(os.path.join(directory_path_aniso, "*"))

  for file in dir_list:
    if args.base_mesh[:-4] in file:
      outputDir = file

  file_list = glob.glob(os.path.join(outputDir, "*"))

  with open(f"{outputDir}/hausdorff_{args.base_mesh[:-4]}_to_{target_n_faces}.txt", "w") as output_file:
    output_file.write("microFaces RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

    for displaced_mesh_path in file_list:
      ms = pymeshlab.MeshSet()
      ms.load_new_mesh(target_mesh_path)
      ms.load_new_mesh(displaced_mesh_path)
      output_file.write(str(extract_last_integer_number_from_string(displaced_mesh_path)) + " ")

      for key, value in ms.get_hausdorff_distance().items():
        output_file.write(str(value) + " ")

      output_file.write("\n")
  output_file.close()

  print(f"Hausdorff's distances > {outputDir}/hausdorff_microfaces_aniso.txt")
