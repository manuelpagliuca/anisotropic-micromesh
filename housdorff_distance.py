import os
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

# 
directory_path_micro = "./Samples/displaced/micro"
directory_path_aniso = "./Samples/displaced/aniso"
target_file = "pallas_5000.obj"
target_mesh_path = "./Samples/" + target_file

exe_path = os.path.join(os.getcwd(), "Debug", "master_thesis.exe")

delete_files_in_directory(directory_path_micro)
delete_files_in_directory(directory_path_aniso)

command_to_execute = " comparison_samples n=7 min_val=1.0 max_val=7.0 target_faces=5000"
params = ["comparison_samples", "n=7", "min_val=0.3", "max_val=7.0", "target=" + target_file]

try:
	subprocess.check_call([exe_path, params[0], params[1], params[2], params[3], params[4]])
except subprocess.CalledProcessError as e:
	print(f"Error while executing the command: {e}")
except FileNotFoundError as e:
	print(f"File not found: {e}")

# come prima cosa dovrei eseguire la mia applicazione da linea di comando in
# maniera che generi i sample in cui sono interessato

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
