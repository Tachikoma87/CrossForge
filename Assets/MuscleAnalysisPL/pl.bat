@echo %CD%
python bvhtotrc.py animation.bvh
python bvhscale.py animation.bvh
"C:\Program Files\Blender Foundation\Blender 2.92\blender.exe" -b --python ".\convert_fbx.py" -- "animation_scaled.bvh"
opensim-cmd run-tool ik.xml
opensim-cmd run-tool analyze.xml