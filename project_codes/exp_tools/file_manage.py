import os
import shutil



def deal_core_dump_files():
    current_directory = os.getcwd()
    core_file_dir=current_directory+"/core_dump_files"
    if os.path.exists(core_file_dir):
        shutil.rmtree(core_file_dir)    
    os.makedirs(core_file_dir)
    files = os.listdir(current_directory)

    for file in files:
        if file.startswith("core.") and os.path.isfile(os.path.join(current_directory, file)):
            shutil.move(os.path.join(current_directory, file), os.path.join(core_file_dir, file))
            print(f"Moved {file} to {core_file_dir}")

def clean_pyc_files():
    current_directory = os.getcwd()
    directory = current_directory+"/exp_tools"
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".pyc"):
                pyc_file = os.path.join(root, file)
                os.remove(pyc_file)
                # print(f"Removed {pyc_file}")