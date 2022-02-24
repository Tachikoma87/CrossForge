@echo off
cd "C:/Users/Megaport/Documents/STUDIUM/CrossForge/crossForge/CrossForge/Assets/MuscleAnalysisPL"
python bvhtotrc.py animation.bvh
python bvhscale.py animation.bvh
"C:\Program Files\Blender Foundation\Blender 2.92\blender.exe" -b --python ".\convert_fbx.py" -- "animation_scaled.bvh"
opensim-cmd run-tool scale.xml
opensim-cmd run-tool ik.xml
opensim-cmd run-tool analyze.xml