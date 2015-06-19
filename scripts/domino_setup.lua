engine = ScriptEngine.new();

-- Positions.
CENTER = { x = 0, y = 0, z = 0};
TOP_DOWN_DIRECTION = {x = 0, y = -1, z = 0};
-- Color.
BLACK = {r = 0, g = 0, b = 0, a = 1};
WHITE = {r = 1, g = 1, b = 1, a = 1};

--------------------------------------------------------------------------------
function setCamera(camera)
  if camera.position == nil then
    camera.position = CENTER;
  end
  if camera.orientation == nil then
    camera.orientation = CENTER;
  end
  if camera.viewAngle == nil then 
    camera.viewAngle = 70;
  end 
  if camera.zNear == nil then
    camera.zNear = 0.1;
  end 
  if camera.zFar == nil then
    camera.zFar = 2000;
  end

  -- Check fields of position.
  if type(camera.position.x) ~= "number" then
    error("No x coordinate in camera position.");
  elseif type(camera.position.y) ~= "number" then
    error("No y coordinate in camera position.");
  elseif type(camera.position.z) ~= "number" then
    error("No z coordinate in camera position.");
  end
  -- Check fields of orientation.
  if type(camera.orientation.x) ~= "number" then
    error("No x coordinate in camera orientation.");
  elseif type(camera.orientation.y) ~= "number" then
    error("No y coordinate in camera orientation.");
  end

  engine:_setCamera(camera.position.x, camera.position.y, camera.position.z,
                    camera.orientation.x, camera.orientation.y, 
                    camera.viewAngle, camera.zNear, camera.zFar);
end

--------------------------------------------------------------------------------
function setGravity(gravity)
  -- Check fields of position.
  if type(gravity.x) ~= "number" then
    error("No x coordinate in gravity.");
  elseif type(gravity.y) ~= "number" then
    error("No y coordinate in gravity.");
  elseif type(gravity.z) ~= "number" then
    error("No z coordinate in gravity.");
  end

  engine:_setGravity(gravity.x, gravity.y, gravity.z);
end
  
--------------------------------------------------------------------------------
function setBackgroundColor(color) 
  -- Check fields of position.
  if type(color.r) ~= "number" then
    error("No r coordinate in color.");
  elseif type(color.g) ~= "number" then
    error("No g coordinate in color.");
  elseif type(color.b) ~= "number" then
    error("No b coordinate in color.");
  elseif type(color.a) ~= "number" then
    error("No a coordinate in color.");
  end

  engine:_setBackgroundColor(color.r, color.g, color.b, color.a);
end

----------------------------------------------------------------------------------
--function addPositionalLight(light)
--  if light.position == nil then
--    light.position = CENTER;
--  end
--  if light.ambientColor == nil then
--    light.ambientColor = WHITE; 
--  end
--  if light.diffuseColor == nil then
--    light.diffuseColor = WHITE; 
--  end 
--  if light.specularColor == nil then
--    light.specularColor = WHITE; 
--  end 
--  if light.constantAttenuation == nil then
--    light.constantAttenuation = 1;
--  end
--  if light.linearAttenuation == nil then
--    light.linearAttenuation = 0;
--  end
--  if light.quadraticAttenuation == nil then
--    light.quadraticAttenuation = 0;
--  end
--
--  engine:_addPositionalLight(light.position.x,
--                             light.position.y,
--                             light.position.z,
--                             light.ambientColor.r,
--                             light.ambientColor.g,
--                             light.ambientColor.b,
--                             light.ambientColor.a,
--                             light.diffuseColor.r,
--                             light.diffuseColor.g,
--                             light.diffuseColor.b,
--                             light.diffuseColor.a,
--                             light.specularColor.r,
--                             light.specularColor.g,
--                             light.specularColor.b,
--                             light.specularColor.a,
--                             light.constantAttenuation,
--                             light.linearAttenuation,
--                             light.quadraticAttenuation); 
--end

--------------------------------------------------------------------------------
function addDirectionalLight(light)
  if light.direction == nil then
    light.direction = TOP_DOWN_DIRECTION;
  end
  if light.ambientColor == nil then
    light.ambientColor = WHITE; 
  end
  if light.diffuseColor == nil then
    light.diffuseColor = WHITE; 
  end 
  if light.specularColor == nil then
    light.specularColor = WHITE; 
  end 

  engine:_addDirectionalLight(light.direction.x,
                              light.direction.y,
                              light.direction.z,
                              light.ambientColor.r,
                              light.ambientColor.g,
                              light.ambientColor.b,
                              light.ambientColor.a,
                              light.diffuseColor.r,
                              light.diffuseColor.g,
                              light.diffuseColor.b,
                              light.diffuseColor.a,
                              light.specularColor.r,
                              light.specularColor.g,
                              light.specularColor.b,
                              light.specularColor.a);

end

--------------------------------------------------------------------------------
function addSpotLight(light)
  if light.position == nil then
    light.position = CENTER;
  end
  if light.direction == nil then
    light.direction = TOP_DOWN_DIRECTION;
  end
  if light.ambientColor == nil then
    light.ambientColor = WHITE; 
  end
  if light.diffuseColor == nil then
    light.diffuseColor = WHITE; 
  end 
  if light.specularColor == nil then
    light.specularColor = WHITE; 
  end 
  if light.constantAttenuation == nil then
    light.constantAttenuation = 1;
  end
  if light.linearAttenuation == nil then
    light.linearAttenuation = 0;
  end
  if light.quadraticAttenuation == nil then
    light.quadraticAttenuation = 0;
  end
  if light.exponent == nil then
    light.exponent = 1;
  end
  if light.cutoff == nil then
    light.cutoff = 1;
  end

  engine:_addSpotLight(light.position.x,
                       light.position.y,
                       light.position.z,
                       light.direction.x,
                       light.direction.y,
                       light.direction.z,
                       light.ambientColor.r,
                       light.ambientColor.g,
                       light.ambientColor.b,
                       light.ambientColor.a,
                       light.diffuseColor.r,
                       light.diffuseColor.g,
                       light.diffuseColor.b,
                       light.diffuseColor.a,
                       light.specularColor.r,
                       light.specularColor.g,
                       light.specularColor.b,
                       light.specularColor.a,
                       light.constantAttenuation,
                       light.linearAttenuation,
                       light.quadraticAttenuation,
                       light.exponent,
                       light.cutoff); 
end

--------------------------------------------------------------------------------
function addMirror(mirror)
  -- Mandatory parameters.
  if mirror.sides == nil then
    error("Mirror sides missing");
  end

  -- Optional parameters.
  if mirror.mass == nil then
    mirror.mass = 0;
  end
  if mirror.position == nil then
    mirror.position = CENTER;
  end
  if mirror.orientation == nil then
    mirror.orientation = CENTER;
  end

  engine:_addMirror(mirror.sides.x,
                    mirror.sides.y,
                    mirror.sides.z,
                    mirror.mass,
                    mirror.position.x,
                    mirror.position.y,
                    mirror.position.z,
                    mirror.orientation.x,
                    mirror.orientation.y);
end

--------------------------------------------------------------------------------
function addPlane(plane)
  -- Mandatory parameters.
  if plane.side == nil then
    error("Plane side missing");
  end
  if plane.textureFile == nil then
    error("Plane texture missing");
  end

  -- Optional parameters.
  if plane.mass == nil then
    plane.mass = 0;
  end
  if plane.position == nil then
    plane.position = CENTER;
  end
  if plane.orientation == nil then
    plane.orientation = CENTER;
  end
  if plane.ambientColor == nil then
    plane.ambientColor = BLACK;
  end
  if plane.diffuseColor == nil then
    plane.diffuseColor = BLACK; 
  end 
  if plane.specularColor == nil then
    plane.specularColor = BLACK; 
  end 
  if plane.shader == nil then
    plane.shader = "phongNormalMapping";
  end
  if plane.normalTextureFile == nil then
    plane.normalTextureFile = "";
  end
  if plane.textureRepetitions == nil then
    plane.textureRepetitions = 1;
  end

  engine:_addPlane(plane.side,
                   plane.mass,
                   plane.position.x,
                   plane.position.y,
                   plane.position.z,
                   plane.orientation.x,
                   plane.orientation.y,
                   plane.orientation.z,
                   plane.ambientColor.r,
                   plane.ambientColor.g,
                   plane.ambientColor.b,
                   plane.ambientColor.a,
                   plane.diffuseColor.r,
                   plane.diffuseColor.g,
                   plane.diffuseColor.b,
                   plane.diffuseColor.a,
                   plane.specularColor.r,
                   plane.specularColor.g,
                   plane.specularColor.b,
                   plane.specularColor.a,
                   plane.textureFile,
                   plane.normalTextureFile,
                   plane.textureRepetitions,
                   plane.shader);
end

--------------------------------------------------------------------------------
function addBox(box)
  -- Mandatory parameters.
  if box.sides == nil then
    error("Box sides missing");
  end

  -- Optional parameters.
  if box.mass == nil then
    box.mass = 0;
  end
  if box.position == nil then
    box.position = CENTER;
  end
  if box.orientation == nil then
    box.orientation = CENTER;
  end
  if box.ambientColor == nil then
    box.ambientColor = BLACK;
  end
  if box.diffuseColor == nil then
    box.diffuseColor = BLACK; 
  end 
  if box.specularColor == nil then
    box.specularColor = BLACK; 
  end 
  if box.shader == nil then
    box.shader = "phongNormalMapping";
  end
  if box.textureFile == nil then
    box.textureFile = "";
  end
  if box.normalTextureFile == nil then
    box.normalTextureFile = "";
  end

  engine:_addBox(box.sides.x,
                 box.sides.y,
                 box.sides.z,
                 box.mass,
                 box.position.x,
                 box.position.y,
                 box.position.z,
                 box.orientation.x,
                 box.orientation.y,
                 box.orientation.z,
                 box.ambientColor.r,
                 box.ambientColor.g,
                 box.ambientColor.b,
                 box.ambientColor.a,
                 box.diffuseColor.r,
                 box.diffuseColor.g,
                 box.diffuseColor.b,
                 box.diffuseColor.a,
                 box.specularColor.r,
                 box.specularColor.g,
                 box.specularColor.b,
                 box.specularColor.a,
                 box.textureFile,
                 box.normalTextureFile,
                 box.shader);
end

--------------------------------------------------------------------------------
function addLightBulb(bulb)
  if bulb.radius == nil then
    error("Light bulb radius missing");
  end
  -- Optional parameters.
  if bulb.mass == nil then
    bulb.mass = 0;
  end
  if bulb.position == nil then
    bulb.position = CENTER;
  end
  if bulb.orientation == nil then
    bulb.orientation = CENTER;
  end
  if bulb.ambientColor == nil then
    bulb.ambientColor = BLACK;
  end
  if bulb.diffuseColor == nil then
    bulb.diffuseColor = WHITE; 
  end 
  if bulb.specularColor == nil then
    bulb.specularColor = WHITE; 
  end 
  if bulb.constantAttenuation == nil then
    bulb.constantAttenuation = 1;
  end
  if bulb.linearAttenuation == nil then
    bulb.linearAttenuation = 0;
  end
  if bulb.quadraticAttenuation == nil then
    bulb.quadraticAttenuation = 0;
  end
  if bulb.textureFile == nil then
    bulb.textureFile = "";
  end

  bulb.shader = "lightBulb";

  engine:_addLightBulb(bulb.radius,
                       bulb.mass,
                       bulb.position.x,
                       bulb.position.y,
                       bulb.position.z,
                       bulb.orientation.x,
                       bulb.orientation.y,
                       bulb.orientation.z,
                       bulb.ambientColor.r,
                       bulb.ambientColor.g,
                       bulb.ambientColor.b,
                       bulb.ambientColor.a,
                       bulb.diffuseColor.r,
                       bulb.diffuseColor.g,
                       bulb.diffuseColor.b,
                       bulb.diffuseColor.a,
                       bulb.specularColor.r,
                       bulb.specularColor.g,
                       bulb.specularColor.b,
                       bulb.specularColor.a,
                       bulb.constantAttenuation,
                       bulb.linearAttenuation,
                       bulb.quadraticAttenuation,
                       bulb.textureFile,
                       bulb.shader);
end

--------------------------------------------------------------------------------
function addMesh(mesh)
  -- Mandatory parameters.
  if mesh.objFile == nil then
    error("Mesh objFile missing");
  end

  -- Optional parameters.
  if mesh.mass == nil then
    mesh.mass = 0;
  end
  if mesh.position == nil then
    mesh.position = CENTER;
  end
  if mesh.orientation == nil then
    mesh.orientation = CENTER;
  end
  if mesh.ambientColor == nil then
    mesh.ambientColor = BLACK;
  end
  if mesh.diffuseColor == nil then
    mesh.diffuseColor = BLACK; 
  end 
  if mesh.specularColor == nil then
    mesh.specularColor = BLACK; 
  end 
  if mesh.shader == nil then
    mesh.shader = "phong";
  end

  engine:_addMesh(mesh.objFile,
                  mesh.mass,
                  mesh.position.x,
                  mesh.position.y,
                  mesh.position.z,
                  mesh.orientation.x,
                  mesh.orientation.y,
                  mesh.orientation.z,
                  mesh.ambientColor.r,
                  mesh.ambientColor.g,
                  mesh.ambientColor.b,
                  mesh.ambientColor.a,
                  mesh.diffuseColor.r,
                  mesh.diffuseColor.g,
                  mesh.diffuseColor.b,
                  mesh.diffuseColor.a,
                  mesh.specularColor.r,
                  mesh.specularColor.g,
                  mesh.specularColor.b,
                  mesh.specularColor.a,
                  mesh.shader);
end
