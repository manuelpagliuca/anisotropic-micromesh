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

# Parse the args
parser = argparse.ArgumentParser(description="Description of your application.")
parser.add_argument("-n", type=int, default=7, help="Parameter 2 (n)")
parser.add_argument("--min_val", type=float, default=0.3, help="Parameter 3 (min_val)")
parser.add_argument("--max_val", type=float, default=7.0, help="Parameter 4 (max_val)")
parser.add_argument("--target", default="pallas_5000.obj", help="Parameter 5 (target)")

args = parser.parse_args()

# Setup path variables
directory_path_micro = "./Samples/displaced/micro"
directory_path_aniso = "./Samples/displaced/aniso"
target_mesh_path = "./Samples/" + args.target

exe_path = os.path.join(os.getcwd(), "Debug", "master_thesis.exe")

# Clean the samples dirs
delete_files_in_directory(directory_path_micro)
delete_files_in_directory(directory_path_aniso)

# Execute the command
params = [
	"build-samples",
	f"n={args.n}",
	f"min_val={args.min_val}",
	f"max_val={args.max_val}",
	f"target={args.target}",
]

try:
	subprocess.check_call([exe_path, params[0], params[1], params[2], params[3], params[4]])
except subprocess.CalledProcessError as e:
	print(f"Error while executing the command: {e}")
except FileNotFoundError as e:
	print(f"File not found: {e}")

file_list = glob.glob(os.path.join(directory_path_micro, "*"))

with open("./hausdorff_edge_length_micro.txt", "w") as output_file:
	output_file.write("edgeLength RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

	for displaced_mesh_path in file_list:
		ms = pymeshlab.MeshSet()
		ms.load_new_mesh(target_mesh_path)
		ms.load_new_mesh(displaced_mesh_path)
		output_file.write(str(extract_last_decimal_number_from_string(displaced_mesh_path)) + " ")

		for key, value in ms.get_hausdorff_distance().items():
			output_file.write(str(value) + " ")

		output_file.write("\n")

print("Hausdorff's distances for micro-mesh scheme > ./hausdorff_edge_length_micro.txt")

file_list = glob.glob(os.path.join(directory_path_aniso, "*"))

with open("./hausdorff_edge_length_aniso.txt", "w") as output_file:
	output_file.write("edgeLength RMS diag_mesh_0 diag_mesh_1 max mean min n_samples\n")

	for displaced_mesh_path in file_list:
		ms = pymeshlab.MeshSet()
		ms.load_new_mesh(target_mesh_path)
		ms.load_new_mesh(displaced_mesh_path)
		output_file.write(str(extract_last_decimal_number_from_string(displaced_mesh_path)) + " ")

		for key, value in ms.get_hausdorff_distance().items():
			output_file.write(str(value) + " ")

		output_file.write("\n")
print("Hausdorff's distances for anisotropic micro-mesh scheme > ./hausdorff_edge_length_micro.txt")

output_file.close()

print("Hausdorff's distances are being computed and exported successfully.")
