#include "Mesh.h"

#include <boost/tokenizer.hpp>

#include <LinearMath/btVector3.h>

#include <map>

#include <glm/ext.hpp>

#include "SysUtils.h"

template class ObjectBuilder<MeshBuilder>;

Mesh::Mesh(btTransform &transform, btScalar mass, btVector3 &inertia)
    : Object(transform, mass, inertia) {
  // Setup the shape using convex hull.
  btVector3 tmpHalfSides = btVector3(2, 2, 2);
  collisionShape = new btBoxShape(tmpHalfSides);
  collisionShape->calculateLocalInertia(mass, inertia);
  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

//------------------------------------------------------------------------------
// Support functions.
inline glm::vec3 parseFloatTriple(const std::vector<std::string> &words) {
  assert(words.size() == 3 && "Wrong format");
  return glm::vec3(::atof(words[0].c_str()), ::atof(words[1].c_str()),
                   ::atof(words[2].c_str()));
}

//------------------------------------------------------------------------------
MeshBuilder::MeshBuilder() : ObjectBuilder() {}

MeshBuilder &MeshBuilder::setObjFile(const std::string& filePath) {
  this->objFile = filePath;
  return *this;
}

Mesh *MeshBuilder::create() {
  Mesh *mesh = new Mesh(transform, mass, inertia);
  parseObjFile();
  fillMesh(mesh);
  ObjectBuilder::setColors(mesh);
  return mesh;
}

//------------------------------------------------------------------------------
void MeshBuilder::parseObjFile() {
  std::string data = getFileContent(objFile.c_str());

  boost::char_separator<char> newLineSeparator("\n");
  boost::tokenizer<boost::char_separator<char> > tokenizer(data,
                                                           newLineSeparator);
  for (auto line : tokenizer) {
    parseObjLine(line);
  }
}

//------------------------------------------------------------------------------
void MeshBuilder::parseObjLine(const std::string &line) {
  boost::char_separator<char> spaceSeparator(" ");
  boost::tokenizer<boost::char_separator<char> > tokens(line, spaceSeparator);

  std::vector<std::string> words;
  std::copy(tokens.begin(), tokens.end(),
            std::back_inserter<std::vector<std::string> >(words));

  std::string firstWord = words[0];

  if (firstWord == "v")
    parseVertex(std::vector<std::string>(words.begin() + 1, words.end()));

  if (firstWord == "f")
    parseFace(std::vector<std::string>(words.begin() + 1, words.end()));

  if (firstWord == "vn")
    parseNormal(std::vector<std::string>(words.begin() + 1, words.end()));

  if (firstWord == "mtllib")
    materialFile = words[1];

  if (firstWord == "o")
    objName = words[1];

  if (firstWord == "usemtl")
    materialName = words[1];
}

void MeshBuilder::parseNormal(const std::vector<std::string> &words) {
  assert(words.size() == 3 && "Wrong normal format");
  normals.push_back(parseFloatTriple(words));
}

void MeshBuilder::parseVertex(const std::vector<std::string> &words) {
  assert(words.size() == 3 && "Wrong normal format");
  points.push_back(parseFloatTriple(words));
}

void MeshBuilder::parseFace(const std::vector<std::string> &words) {
  assert(words.size() == 3 && "Wrong format");
  for (auto word : words) {
    boost::char_separator<char> slashSeparator("/", "",
                                               boost::keep_empty_tokens);
    boost::tokenizer<boost::char_separator<char> > tokens(word, slashSeparator);
    std::vector<std::string> values;
    values.reserve(3);
    std::copy(tokens.begin(), tokens.end(),
              std::back_inserter<std::vector<std::string> >(values));

    int vertexIndex = ::atoi(values[0].c_str());
    int normalIndex = ::atoi(values[2].c_str());
    int textureIndex = 0;
    if (values[1] != "")
      textureIndex = ::atoi(values[1].c_str());

    indices.emplace_back(std::make_tuple(vertexIndex, textureIndex, normalIndex));
  }
}

void MeshBuilder::fillMesh(Mesh *mesh) {
  std::map<FaceIndices, int> indexMap;

  const int indexNumber = indices.size();

  int counter = 0;

  for (int index = 0; index < indexNumber; ++index) {
    FaceIndices faceIndices = indices[index];
    glm::vec3 vertex = points[std::get<0>(faceIndices) - 1];
    glm::vec3 normal = normals[std::get<2>(faceIndices) - 1];

    auto iterator = indexMap.find(faceIndices);
    if (iterator != indexMap.end()) {
      mesh->indices.push_back(iterator->second);
    } else {
      mesh->points.push_back(vertex);
      mesh->normals.push_back(normal);
      mesh->indices.push_back(counter);
      indexMap[faceIndices] = counter;
      counter++;
    }
  }
}
