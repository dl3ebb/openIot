Import("env")
from SCons.Script import DefaultEnvironment
import os
import subprocess

env = DefaultEnvironment()

# Path to the build script
script_path = os.path.join("scripts", "build_spiffs.sh")

# Run the script
result = subprocess.run([script_path], shell=True)

# Check for errors
if result.returncode != 0:
    print("Error: build_spiffs.sh script failed.")
    env.Exit(1)


