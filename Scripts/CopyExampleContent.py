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
    source = "Examples/Pong/Content/Build"
    target = "build/Debug/bin/Examples/Pong/Content"
    replace_directory(target, source)

    source = "Examples/Pong/Scripts"
    target = "build/Debug/bin/Examples/Pong/Scripts"
    replace_directory(target, source)

    source = "Examples/Pong/Scenes"
    target = "build/Debug/bin/Examples/Pong/Scenes"
    replace_directory(target, source)


example_pong()

print("Done.")
