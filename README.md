# Module 5: Networked Physics
This module demonstrates physics networked between two instances of the game engine. One instance performs the physics simulation, and the other reflects the results.
## Note:
Not currently finished. Still need to work on networking and add some comments here and there.
## Instructions
- First, run one instance of the module with NetServerListenPort=12683 in the aftr.conf file and then run another instance with NetServerListenPort=12682 (or any other port) in the aftr.conf file.
- In the server instance, ctrl+click on any area of the terrain to spawn a teapot above the point clicked.
- All physics in the server instance will be networked to the client instance.