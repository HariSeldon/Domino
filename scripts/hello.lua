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


setCamera({position = {x = 1, y = 1, z = 1}, orientation = {x = 0, y = 0}});
setGravity({x = 0, y = -9.81, z = 0});
setBackgroundColor({r = 0.2, g = 0.4, b = 0.6, a = 1});
--addPositionalLight({position = {x = 0, y = 10, z = 0}, constantAttenuation = 2});
addDirectionalLight({direction = {x = -1, y = -1, z = 0}});
addPlane({side = 60, textureFile = "default.png"});
addMesh({objFile = "barrel.obj", position = {x = 0, y = 20, z = 0.5},
         orientation = {x = 10, y = 4, z = 8}, mass = 10});
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
