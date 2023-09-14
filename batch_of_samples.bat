@echo off
python ./compute_distances.py --base-mesh=Mars_60152.obj --target-mesh=Mars.obj
python ./compute_distances.py --base-mesh=orc_bust_15000.obj --target-mesh=orc_bust.obj
python ./compute_distances.py --base-mesh=scan-the-world-michelangelo-s-david_9847.obj --target-mesh=scan-the-world-michelangelo-s-david.obj
python ./compute_distances.py --base-mesh=Dark_Finger_Reef_Crab_21416.obj --target-mesh=Dark_Finger_Reef_Crab.obj
pause
