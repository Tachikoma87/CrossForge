@echo off
FOR %%f IN (*.bvh) DO "C:\Program Files\Blender Foundation\Blender 2.92\blender.exe" -b --python "C:\Users\Megaport\Documents\STUDIUM\Bachelorarbeit\convert_fbx.py" -- "%%f"