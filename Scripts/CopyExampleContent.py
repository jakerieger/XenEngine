import os
import shutil


def replace_directory(target_path, source_path):
    """
    Deletes the target directory if it exists, then copies the source directory
    to the target path and renames it to match the target directory name.

    :param target_path: Path of the directory to be replaced
    :param source_path: Path of the directory to copy and rename
    """
    # Ensure the target directory is deleted if it exists
    if os.path.exists(target_path):
        shutil.rmtree(target_path)
        print(f"Deleted existing directory: {target_path}")

    # Copy the source directory to the target path
    shutil.copytree(source_path, target_path)
    print(f"Copied directory: {source_path} to {target_path}")


source = "../Examples/Pong/Content/Build"
dest = "../build/Debug/bin/Examples/Pong/Content"
replace_directory(dest, source)