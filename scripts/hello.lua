require "domino_setup"

function signum(x)
   if x<0 then
     return -1
   elseif x>0 then
     return 1
   else
     return 0
   end
end

function traceDominoLine(start, finish)
  DOMINO_NUMBER = 31;

  deltaX = finish.x - start.x;
  deltaZ = finish.z - start.z;

  length = math.sqrt(deltaX * deltaX + deltaZ * deltaZ);
  slope = deltaZ / deltaX;
  if(slope < 0) then
    slope = -1 * slope; 
  end
  if(deltaX == 0) then
    slope = 1;
  end
  angle = math.atan2(deltaZ, deltaX);

  dominoDistance = length / (DOMINO_NUMBER - 1);

  directionX = signum(deltaX);
  directionY = 0;
  directionZ = signum(deltaZ) * slope; 

  directionNorm = math.sqrt(directionX * directionX + directionY * directionY + directionZ * directionZ);

  direction = {x = directionX / directionNorm, y = 0, z = directionZ / directionNorm};

  for index = 0,DOMINO_NUMBER - 1 do 
    positionX = start.x + dominoDistance * index * direction.x; 
    positionY = start.y + dominoDistance * index * direction.y + 1.25;
    positionZ = start.z + dominoDistance * index * direction.z;

    print(positionX, positionY, positionZ);

    addBox({sides = {x = 0.25, y = 2.5, z =1}, 
            position = {x = positionX, y = positionY, z = positionZ}, 
            orientation = {x = -angle, y = 0, z = 0},
            mass = 10, textureFile = "checker.png"});
  end


end

setCamera({position = {x = 0, y = 2, z = -20}, orientation = {x = 0, y = 0}});
setGravity({x = 0, y = -9.81, z = 0});
setBackgroundColor({r = 0.2, g = 0.4, b = 0.6, a = 1});
--addPositionalLight({position = {x = 20, y = 1, z = 0}, constantAttenuation = 1});
--addPositionalLight({position = {x = 19, y = 1, z = 5}, constantAttenuation = 1});
--addSpotLight({position = {x = 0, y = 30, z = 0}, direction = {x = 0, y = -1, z = 0}, 
--              constantAttenuation = 1, cutoff = 85, exponent = 10, constantAttenuation = 2});
--addPositionalLight({position = {x = 0, y = 10, z = 0}, constantAttenuation = 1});
--addPositionalLight({position = {x = 0, y = 0.1, z = 20}, constantAttenuation = 0.1});
--addPositionalLight({position = {x = 20, y = 0.1, z = 20}, constantAttenuation = 0.1});
--addDirectionalLight({direction = {x = -1, y = -1, z = 0}});
--addBox({sides = {x = 100, y = 100, z = 100}, 
--        position = {x = 0, y = 0, z = 0}, 
--        orientation = {x = 0, y = 0, z = 0},
--        mass = 0, textureFile = "default.png"});
addBox({sides = {x = 2, y = 2, z = 2}, 
        position = {x = 0, y = 7, z = 0}, 
        orientation = {x = 0, y = 0, z = 0},
        mass = 0.01, textureFile = "checkerboard.png"});
addLightBulb({side = 1, ambientColor = {r = 0.2, g = 0.2, b = 0.2, a = 1}, 
              position = {x = 1, y = 1, z = 0}, 
              mass = 10, constantAttenuation = 0.1, linearAttenuation = 0.1});
addLightBulb({side = 1, 
              position = {x = 0, y = 2, z = 0}, 
              mass = 0, constantAttenuation = 0.1, linearAttenuation = 0.1});
addLightBulb({side = 1, 
              position = {x = -10, y = 3, z = -10}, 
              mass = 10, constantAttenuation = 0.1, linearAttenuation = 0.1});
addPlane({side = 60, textureFile = "grass.tif", textureRepetitions = 7});

-- for index = 1,20 do
--   addMesh({objFile = "barrel.obj", position = {x = -30 + index * 3, y = 2, z = -30}, mass = 0});
-- end
-- for index = 1,20 do
--   addMesh({objFile = "barrel.obj", position = {x = -30 + index * 3, y = 2, z = 30}, mass = 0});
-- end
-- for index = 1,20 do
--   addMesh({objFile = "barrel.obj", position = {x = 30, y = 2, z = -30 + index * 3}, mass = 0});
-- end
-- for index = 1,20 do
--   addMesh({objFile = "barrel.obj", position = {x = -30, y = 2, z = -30 + index * 3}, mass = 0});
-- end

--for index = 1,30 do 
--  addMesh({objFile = "barrel.obj", position = {x = 0, y = index * 10, z = index % 2}, mass = 10});
--end
--for index = 1,30 do 
--  addMesh({objFile = "kufel.obj", position = {x = 0, y = index * 10, z = index % 2}, mass = 10});
--end

--addMesh({objFile = "torch.obj", position = {x = 0, y = 0, z = 0}, mass = 10, shader = "gouraud"});
--addMesh({objFile = "barrel.obj", position = {x = -10, y = 20, z = 0}, mass = 10});
--addMesh({objFile = "barrel.obj", position = {x = -10, y = 30, z = 0}, mass = 10});
--
----traceDominoLine({x = 1, y = 20, z = 1}, {x = 20, y = 0, z = 20});
----traceDominoLine({x = -1, y = 0, z = 1}, {x = -20, y = 0, z = 20});
----traceDominoLine({x = 20, y = 0, z = 21}, {x = 1, y = 0, z = 80});
----traceDominoLine({x = -20, y = 0, z = 21}, {x = -1, y = 0, z = 80});
--addBox({sides = {x = 1, y = 2.5, z =2}, 
--        position = {x = 0, y = 20, z = -0.5}, 
--        orientation = {x = 0, y = 0, z = 0},
--        mass = 10, textureFile = "checker.png"});
----
----addBox({sides = {x = 0.25, y = 2.5, z =1}, 
----        position = {x = 0, y = 1.25, z = 80.5}, 
----        orientation = {x = 1.57, y = 0, z = 0},
----        mass = 10, textureFile = "checker.png"});
--
----traceDominoLine({x = 20, y = 0, z = 1}, {x = 20, y = 0, z = 21});
----traceDominoLine({x = -20, y = 0, z = 22}, {x = 20, y = 0, z = 22});
