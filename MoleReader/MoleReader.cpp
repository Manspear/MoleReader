#include <fstream>
#include <vector>
#include <iostream>
//#include "ReadHeaderData.h"
#include "MoleReader.h"

using namespace std;


	void MoleReader::readFromBinary(const char* filePath)
	{
		/*Reading the binary file that we just have been written to.*/

		std::ifstream infile(filePath, std::ifstream::binary);

		if (infile.is_open())
		{
			//std::cout << ">>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<" << "\n" << "\n" << endl;
			//std::cout << "Binary Reader" << endl;
			//std::cout << "\n" << endl;

			/*Reading the first block of memory that is the main header. This will read
			information about how much of each node type we have from a imported scene and
			how memory they will take up in the binary file.*/

			infile.read((char*)&pmRead_mainHeader, sizeof(sMainHeader));

			//std::cout << "______________________" << endl;
			//std::cout << "Main Header" << endl;
			//std::cout << "meshCount: " << pmRead_mainHeader.meshCount << endl;
			//std::cout << "materialCount: " << pmRead_mainHeader.materialCount << endl;
			//std::cout << "lightCount: " << pmRead_mainHeader.lightCount << endl;
			//std::cout << "cameraCount: " << pmRead_mainHeader.cameraCount << endl;

			//std::cout << "______________________" << endl;

			if (pmRead_mainHeader.meshCount >= 1)
			{
				pmRead_meshList.resize(pmRead_mainHeader.meshCount + prevMeshes);

				//Reize to be the same length as the mesh list. Must be this way to work "in parallell" 
				//with the mesh list   
				pmRead_meshJointHolder.resize(pmRead_mainHeader.meshCount + prevMeshes);
				pmRead_meshChildList.resize(pmRead_mainHeader.meshCount + prevMeshes);

				//The vertex lists will be filled so that they are the same length as the mesh list.
				pmRead_mList.resize(pmRead_mainHeader.meshCount + prevMeshes);
				pmRead_mkList.resize(pmRead_mainHeader.meshCount + prevMeshes);

				for (int i = 0; i < pmRead_mainHeader.meshCount; i++)
				{
					/*Reading the block of memory that is the meshes. The information from the meshes
					will be read here, that includes for example vertex count for a normal mesh
					and a skinned mesh. What we do is reserving memory for all the data that is in the
					struct. For example, Vertex count is a integer and will take up to 4 bytes in the
					memory when reading.*/

					//read_sMesh m;
					//infile.read((char*)&m, sizeof(read_sMesh));
					//pmRead_meshList.push_back(m);
					infile.read((char*)&pmRead_meshList[i + prevMeshes], sizeof(sMesh));

					int currMeshIndex = i + prevMeshes;
					{
					/*	std::cout << "Mesh: " << currMeshIndex << endl;

						std::cout << "Name: " << pmRead_meshList[currMeshIndex].meshName << endl;

						std::cout << "\t";
						std::cout << "Material ID: ";
						std::cout << pmRead_meshList[currMeshIndex].materialID << endl;

						std::cout << "Mesh vector: " << endl;

						std::cout << "\t";
						std::cout << "xyz: ";
						std::cout << pmRead_meshList[currMeshIndex].translate[0] << " ";
						std::cout << pmRead_meshList[currMeshIndex].translate[1] << " ";
						std::cout << pmRead_meshList[currMeshIndex].translate[2] << " " << endl;

						std::cout << "\t";
						std::cout << "rot: ";
						std::cout << pmRead_meshList[currMeshIndex].rotation[0] << " ";
						std::cout << pmRead_meshList[currMeshIndex].rotation[1] << " ";
						std::cout << pmRead_meshList[currMeshIndex].rotation[2] << " " << endl;

						std::cout << "\t";
						std::cout << "scale: ";
						std::cout << pmRead_meshList[currMeshIndex].scale[0] << " ";
						std::cout << pmRead_meshList[currMeshIndex].scale[1] << " ";
						std::cout << pmRead_meshList[currMeshIndex].scale[2] << " " << endl;

						std::cout << "\t";
						std::cout << "Vertex Count: ";
						std::cout << pmRead_meshList[currMeshIndex].vertexCount << endl;

						std::cout << "\t";
						std::cout << "SkelAnimVert Count: ";
						std::cout << pmRead_meshList[currMeshIndex].skelAnimVertexCount << endl;

						std::cout << "\t";
						std::cout << "Joint Count: ";
						std::cout << pmRead_meshList[currMeshIndex].jointCount << endl;*/
					}


					if (pmRead_meshList[currMeshIndex].isAnimated == true)
					{
						{
							/*std::cout << "\n";
							std::cout << "Skeleton Vertex vector: " << endl;*/

							//std::cout << "mkList: " << endl;
							pmRead_mkList[currMeshIndex].vskList.resize(pmRead_meshList[currMeshIndex].skelAnimVertexCount);
						/*	std::cout << "\t";
							std::cout << pmRead_mkList[currMeshIndex].vskList.data();

							std::cout << "\t";
							std::cout << "Allocated memory for: " << pmRead_meshList[i].skelAnimVertexCount << " skel vertices" << endl << endl;*/
						}
						const int jointCount = pmRead_meshList[currMeshIndex].jointCount;
						/*Reading all the vertex lists for each mesh. For example if a mesh have 200 vertices,
						we can multiply the count of vertices with the sizes in bytes that the sVertex struct have.
						This means that we will be reading the pos, nor, uv, tan, bitan 200 times.*/
						infile.read((char*)pmRead_mkList[currMeshIndex].vskList.data(), sizeof(sSkelAnimVertex) * pmRead_meshList[currMeshIndex].skelAnimVertexCount);

						/*Reading the joint list for each mesh. Every joint in the list have individual data
						that we have to process when reading from the file.*/
						pmRead_meshJointHolder[currMeshIndex].jointList.resize(jointCount);
						pmRead_meshJointHolder[currMeshIndex].perJoint.resize(jointCount);

						//{
						//	std::cout << "\n";
						//	std::cout << "Joint vector: " << endl;

						//	std::cout << "\t";
						//	//	std::cout << pmRead_jointList.data() << endl;

						//	std::cout << "\t";
						//	std::cout << "Allocated memory for: " << pmRead_meshList[currMeshIndex].jointCount << " joints" << endl;
						//}
						/*Reading the data for all the joints that a skinned mesh have.*/

						infile.read((char*)pmRead_meshJointHolder[currMeshIndex].jointList.data(), sizeof(sJoint) * jointCount);

						for (int jointCounter = 0; jointCounter < jointCount; jointCounter++)
						{
							//pmRead_meshJointHolder[currMeshIndex].perJoint[jointCounter].meshChildren.resize()
							const int animStateCount = pmRead_meshJointHolder[currMeshIndex].jointList[jointCounter].animationStateCount;
							pmRead_meshJointHolder[currMeshIndex].perJoint[jointCounter].animationStateTracker.resize(animStateCount);

							infile.read((char*)pmRead_meshJointHolder[currMeshIndex].perJoint[jointCounter].animationStateTracker.data(), sizeof(sAnimationStateTracker) * animStateCount);

							const int meshChildCount = pmRead_meshJointHolder[currMeshIndex].jointList[jointCounter].meshChildCount;
							//here crash
							infile.read((char*)pmRead_meshJointHolder[currMeshIndex].perJoint[jointCounter].meshChildren.data(), sizeof(int) * meshChildCount);

							pmRead_meshJointHolder[currMeshIndex].perJoint[jointCounter].animationStates.resize(animStateCount);

							for (int animStateCounter = 0; animStateCounter < animStateCount; animStateCounter++)
							{
								const int keyCount = pmRead_meshJointHolder[currMeshIndex].perJoint[jointCounter].animationStateTracker[animStateCounter].keyCount;
								pmRead_meshJointHolder[currMeshIndex].perJoint[jointCounter].animationStates[animStateCounter].keyFrames.resize(keyCount);

								infile.read((char*)pmRead_meshJointHolder[currMeshIndex].perJoint[jointCounter].animationStates[animStateCounter].keyFrames.data(), sizeof(sKeyFrame) * keyCount);
							}
						}
					}

					else
					{
						pmRead_mList[currMeshIndex].vList.resize(pmRead_meshList[currMeshIndex].vertexCount);
					/*	std::cout << "\n";
						std::cout << "Vertex vector: " << endl;
						std::cout << "mList: " << endl;
						std::cout << "\t";
						std::cout << pmRead_mList[currMeshIndex].vList.data() << endl;

						std::cout << "\t";
						std::cout << "Allocated memory for " << pmRead_meshList[currMeshIndex].vertexCount << " vertices" << endl << endl;*/

						pmRead_mList[currMeshIndex].vList.resize(pmRead_meshList[currMeshIndex].vertexCount);

						/*Reading all the vertex lists for each mesh. For example if a mesh have 200 vertices,
						we can multiply the count of vertices with the sizes in bytes that the sVertex struct have.
						This means that we will be reading the pos, nor, uv, tan, bitan 200 times.*/
						infile.read((char*)pmRead_mList[currMeshIndex].vList.data(), sizeof(sVertex) * pmRead_meshList[currMeshIndex].vertexCount);
					}
					const int childMeshCount = pmRead_meshList[currMeshIndex].meshChildCount;
					if (childMeshCount > 0)
					{
						pmRead_meshChildList[currMeshIndex].meshChildList.resize(childMeshCount);
						infile.read((char*)pmRead_meshChildList[currMeshIndex].meshChildList.data(), sizeof(sMeshChild) * childMeshCount);
					}
				}
				prevMeshes += pmRead_mainHeader.meshCount;
			}

			if (pmRead_mainHeader.materialCount >= 1)
			{
				pmRead_materialList.resize(pmRead_mainHeader.materialCount + prevMaterials);
				infile.read((char*)pmRead_materialList.data(), sizeof(sMaterial) * (pmRead_mainHeader.materialCount + prevMaterials));

				//pmRead_materialList.resize(pmRead_mainHeader.materialCount + prevMaterials);

				/*for (int i = 0; i < pmRead_mainHeader.materialCount; i++)
				{
				int currIndex = i + prevMaterials;*/

				/*Reading all the materials from the list with the size in bytes in mind.*/
				//	infile.read((char*)&pmRead_materialList[currIndex], sizeof(read_sMaterial));
				/*	read_sMaterial m;
				infile.read((char*)&m, sizeof(read_sMaterial));
				pmRead_materialList.push_back(m);
				{
				std::cout << "Material: " << i << " Name: " << pmRead_materialList[currIndex].materialName << endl;

				std::cout << "Material vector: " << endl;
				std::cout << "\t";
				std::cout << &pmRead_materialList[currIndex] << endl;

				std::cout << "\t";
				std::cout << "Allocated memory for " << pmRead_mainHeader.materialCount << " materials" << endl;

				std::cout << "\t";
				std::cout << "Ambient color: ";
				std::cout << pmRead_materialList[currIndex].ambientColor[0] << " "
				<< pmRead_materialList[currIndex].ambientColor[1] << " "
				<< pmRead_materialList[currIndex].ambientColor[2] << " " << endl;

				std::cout << "\t";
				std::cout << "Diffuse color: ";
				std::cout << pmRead_materialList[currIndex].diffuseColor[0] << " "
				<< pmRead_materialList[currIndex].diffuseColor[1] << " "
				<< pmRead_materialList[currIndex].diffuseColor[2] << " " << endl;

				std::cout << "\t";
				std::cout << "Specular color: ";
				std::cout << pmRead_materialList[currIndex].specularColor[0] << " "
				<< pmRead_materialList[currIndex].specularColor[1] << " "
				<< pmRead_materialList[currIndex].specularColor[2] << " " << endl;

				std::cout << "\t";
				std::cout << "Shiny factor: ";
				std::cout << pmRead_materialList[currIndex].shinyFactor << endl;

				std::cout << "\t";
				std::cout << "Diffuse texture: " << pmRead_materialList[currIndex].diffuseTexture << endl;

				std::cout << "\t";
				std::cout << "Specular texture: " << pmRead_materialList[currIndex].specularTexture << endl;

				std::cout << "\t";
				std::cout << "Normal texture: " << pmRead_materialList[currIndex].normalTexture << endl;


				std::cout << "______________________" << endl;
				}
				}*/

				prevMaterials += pmRead_mainHeader.materialCount;

			}

			if (pmRead_mainHeader.lightCount >= 1)
			{
				pmRead_lightList.resize(pmRead_mainHeader.lightCount + prevLights);

				for (int i = 0; i < pmRead_mainHeader.lightCount; i++)
				{
					int currIndex = i + prevLights;

					/*Reading all the lights from the list with the size in bytes in mind.*/
					infile.read((char*)&pmRead_lightList[currIndex], sizeof(sLight));
					{
					/*	std::cout << "Light: " << i << endl;

						std::cout << "Light vector: " << endl;

						std::cout << "\t";
						std::cout << &pmRead_lightList[currIndex] << endl;

						std::cout << "\t";
						std::cout << "Allocated memory for " << pmRead_mainHeader.lightCount << " lights" << endl;

						std::cout << "\t";
						std::cout << "Light ID: " << pmRead_lightList[currIndex].lightID << endl;

						std::cout << "\t";
						std::cout << "Light position: " << pmRead_lightList[currIndex].lightPos[0] << " "
							<< pmRead_lightList[currIndex].lightPos[1] << " "
							<< pmRead_lightList[currIndex].lightPos[2] << endl;

						std::cout << "\t";
						std::cout << "Light rotation: " << pmRead_lightList[currIndex].lightRot[0] << " "
							<< pmRead_lightList[currIndex].lightRot[1] << " "
							<< pmRead_lightList[currIndex].lightRot[2] << endl;

						std::cout << "\t";
						std::cout << "Light scale: " << pmRead_lightList[currIndex].lightScale[0] << " "
							<< pmRead_lightList[currIndex].lightScale[1] << " "
							<< pmRead_lightList[currIndex].lightScale[2] << endl;

						std::cout << "\t";
						std::cout << "Light color: " << pmRead_lightList[currIndex].color[0] << " "
							<< pmRead_lightList[currIndex].color[1] << " "
							<< pmRead_lightList[currIndex].color[2] << " " << endl;

						std::cout << "\t";
						std::cout << "Light intensity: " << pmRead_lightList[currIndex].intensity << endl;

						std::cout << "______________________" << endl;*/
					}
				}
				prevLights = pmRead_mainHeader.lightCount;
			}

			if (pmRead_mainHeader.cameraCount >= 1)
			{
				pmRead_cameraList.resize(pmRead_mainHeader.cameraCount + prevCameras);

				for (int i = 0; i < pmRead_mainHeader.cameraCount; i++)
				{
					int currIndex = i + prevCameras;

					/*Reading all the cameras from the list with the size in bytes in mind.*/
					infile.read((char*)&pmRead_cameraList[currIndex], sizeof(sCamera));
					{
						/*std::cout << "Camera: " << i << endl;

						std::cout << "Camera vector: " << endl;

						std::cout << "\t";
						std::cout << &pmRead_cameraList[currIndex] << endl;

						std::cout << "\t";
						std::cout << "Allocated memory for " << pmRead_mainHeader.cameraCount << " cameras" << endl;

						std::cout << "\t";
						std::cout << "Camera position: " << pmRead_cameraList[currIndex].camPos[0] << " "
							<< pmRead_cameraList[currIndex].camPos[1] << " "
							<< pmRead_cameraList[currIndex].camPos[2] << endl;

						std::cout << "\t";
						std::cout << "Camera Up vector: " << pmRead_cameraList[currIndex].upVector[0] << " "
							<< pmRead_cameraList[currIndex].upVector[1] << " "
							<< pmRead_cameraList[currIndex].upVector[2] << endl;

						std::cout << "\t";
						std::cout << "FOV: " << pmRead_cameraList[currIndex].fieldOfView << endl;

						std::cout << "\t";
						std::cout << "Near plane: " << pmRead_cameraList[currIndex].nearPlane << endl;

						std::cout << "\t";
						std::cout << "Far plane: " << pmRead_cameraList[currIndex].farPlane << endl;

						std::cout << "______________________" << endl;*/
					}
				}
				prevCameras += pmRead_mainHeader.cameraCount;
			}
			//contains the meshes
			//pmRead_meshList;
			////Contains the vertices for each mesh
			//pmRead_mList;
			////contains skeletal vertices for each mesh
			//pmRead_mkList;
			////contains mesh children for each mesh
			//pmRead_meshChildList;
			////Contains joint and animLayer-data
			//pmRead_meshJointHolder;
			////contains the cameras
			//pmRead_cameraList;
			////contains the lights
			//pmRead_lightList;
			////contains the materials
			//pmRead_materialList;

			infile.close();
		}

		/*pmRead_meshList;
		pmRead_mList;
		pmRead_mkList;
		pmRead_materialList;
		pmRead_meshChildList;*/
		/*int lo = 5;*/
	}

	//const std::vector<read_sMesh>* MoleReader::getMeshList()
	//{
	//	return nullptr;
	//}



	const std::vector<sMesh>* MoleReader::getMeshList()
	{
		return &pmRead_meshList;
	}

	const std::vector<sMChildHolder>* MoleReader::getMeshChildList()
	{
		return &pmRead_meshChildList;
	}

	//const std::vector<read_m>* MoleReader::getVertexList()
	//{
	//	return &pmRead_mList;
	//}
	//
	//const std::vector<read_mk>* MoleReader::getSkeletalVertexList()
	//{
	//	return &pmRead_mkList;
	//}

	const std::vector<sMaterial>* MoleReader::getMaterialList()
	{
		return &pmRead_materialList;
	}

	const std::vector<sCamera>* MoleReader::getCameraList()
	{
		return &pmRead_cameraList;
	}

	const std::vector<sLight>* MoleReader::getLightList()
	{
		return &pmRead_lightList;
	}

	const std::vector<sMJHolder>* MoleReader::getJointKeyList()
	{
		return &pmRead_meshJointHolder;
	}

	const sMainHeader * MoleReader::getMainHeader()
	{
		return &pmRead_mainHeader;
	}

	const int MoleReader::getMeshIndex(string meshName)
	{
		for (int meshIndex = 0; meshIndex < pmRead_mainHeader.meshCount; meshIndex++)
		{
			int check = meshName.compare(pmRead_meshList[meshIndex].meshName);
			if (check == 0)
			{
				return meshIndex;
			}
		}
		return -1337;
	}

	const sMesh * MoleReader::getMesh(int meshIndex)
	{
		return &pmRead_meshList[meshIndex];
	}

	const std::vector<sKeyFrame>* MoleReader::getKeyList(int meshIndex, int jointIndex, int animationState)
	{

		return &pmRead_meshJointHolder[meshIndex].perJoint[jointIndex].animationStates[animationState].keyFrames;
	}

	const std::vector<sMeshChild>* MoleReader::getMeshChildList(int meshIndex)
	{
		return &pmRead_meshChildList[meshIndex].meshChildList;
	}

	const sMaterial * MoleReader::getMaterial(int materialIndex)
	{
		return &pmRead_materialList[materialIndex];
	}

	const sJoint * MoleReader::getJoint(int meshIndex, int jointIndex)
	{
		return &pmRead_meshJointHolder[meshIndex].jointList[jointIndex];
	}

	const std::vector<sMeshChild> MoleReader::getJointMeshChildList(int meshIndex, int jointIndex)
	{
		return pmRead_meshJointHolder[jointIndex].perJoint[jointIndex].meshChildren;
	}

	const std::vector<sSkelAnimVertex>* MoleReader::getSkelVertexList(int meshIndex)
	{
		return &pmRead_mkList[meshIndex].vskList;
	}

	const std::vector<sVertex>* MoleReader::getVertexList(int meshIndex)
	{
		return &pmRead_mList[meshIndex].vList;
	}

	MoleReader::MoleReader()
	{
		prevMeshes = 0;
		prevCameras = 0;
		prevLights = 0;
		prevMaterials = 0;
		prevJoints = 0;
	}
	MoleReader::~MoleReader()
	{

	}


//std::cout << "______________________" << endl;
//GLuint vertexBuff;


//glGenVertexArrays(1, &vao);
//glBindVertexArray(vao);

//// It wörks
//glEnableVertexAttribArray(0);
//glEnableVertexAttribArray(1);
//glEnableVertexAttribArray(2);
//glEnableVertexAttribArray(3);
//glEnableVertexAttribArray(4);

//glGenBuffers(1, &vertexBuff);
//glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);
//glBufferData(GL_ARRAY_BUFFER, sizeof(read_sVertex) * read_mList[i].vList.size(), read_mList[i].vList.data(), GL_STATIC_DRAW);
//std::cout << "______________________" << endl;