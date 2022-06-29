setlocal
cd /d %~dp0
python bvhtotrc.py animation.bvh
python bvhscale.py animation.bvh
"C:\Program Files\Blender Foundation\Blender 2.92\blender.exe" -b --python ".\convert_fbx.py" -- "animation_scaled.bvh"
"C:\OpenSim 4.2\bin\opensim-cmd" run-tool ik.xml
"C:\OpenSim 4.2\bin\opensim-cmd" run-tool analyze.xml