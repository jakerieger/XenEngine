import os
import shutil


def replace_directory(target_path, source_path):
    if os.path.exists(target_path):
        shutil.rmtree(target_path)
        print(f"Deleted existing directory: {target_path}")

    shutil.copytree(source_path, target_path)
    print(f"Copied directory: {source_path} to {target_path}")


print("Copying example content.")


def example_pong():
    source = "Examples/Pong/Build"
    target = "build/Debug/bin/Examples/Pong/Content"
    replace_directory(target, source)


example_pong()

print("Done.")
