require "domino_setup"

setCamera({x = 1, y = 1, z = 1}, {x = 0, y = 0});
setGravity({x = 0, y = -9.81, z = 0});
addPositionalLight({position = {x = 0, y = 5, z = 0}, constantAttenuation = 2});
addPlane({side = 50, textureFile = "default.png"});
addMesh({objFile = "barrel.obj", position = {x = 0, y = 20, z = 0}, mass = 10});
