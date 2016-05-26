#pragma once
#include <vector>
#include <string>
//#include "MoleImporterHeaders.h"
#include <vector>


	struct sMainHeader
	{
		unsigned int meshCount;
		unsigned int materialCount;
		unsigned int lightCount;
		unsigned int cameraCount;
	};

	struct  sMesh
	{
		char meshName[256];

		unsigned int materialID;
		int parentMeshID;
		int parentJointID;

		float translate[3];
		float rotation[3];
		float scale[3];

		bool isBoundingBox;
		bool isAnimated;

		unsigned int meshChildCount = 0;
		unsigned int vertexCount = 0;
		unsigned int skelAnimVertexCount = 0;
		unsigned int jointCount = 0;
	};

	struct  sVertex
	{
		float vertexPos[3];
		float vertexNormal[3];
		float vertexUV[2];
		float tangentNormal[3];
		float biTangentNormal[3];
	};

	struct sMeshChild
	{
		int meshChildID;
	};

	/**
	WIll be held in a vector the same length as mTempMeshList.size();
	Holds a vector containing meshChildren.
	**/
	struct sMChildHolder
	{
		std::vector<sMeshChild> meshChildList;
	};

	/**
	This struct is used to be "parallell" to the
	mesh struct. They are connected implicitly.
	**/
	struct  m
	{
		std::vector<sVertex> vList;
	};

	struct  sSkelAnimVertex
	{
		float vertexPos[3];
		float vertexNormal[3];
		float vertexUV[2];
		float tangentNormal[3];
		float biTangentNormal[3];
		int influences[4];
		float weights[4];
	};

	struct mk
	{
		std::vector<sSkelAnimVertex> vskList;
	};

	struct sJoint
	{
		int jointID;
		int parentJointID;

		float pos[3];
		float rot[3];
		float scale[3];

		float bindPoseInverse[16];
		float globalBindPoseInverse[16];

		int meshChildCount;
		int animationStateCount;
	};

	struct sKeyFrame
	{
		float keyTime;
		float keyPos[3];
		float keyRotate[3];
		float keyScale[3];
	};

	struct sAnimationStateTracker
	{
		int keyCount;
	};

	struct sAnimationState
	{
		std::vector<sKeyFrame> keyFrames;
	};

	/**
	JointHolder --> Holds things for on per-joint basis
	**/
	struct sJHolder
	{
		std::vector<sMeshChild> meshChildren; //resize(mesh[0].joint[0].meshChildCount)
		std::vector<sAnimationStateTracker> animationStateTracker; //resize(mesh[0].joint[0].animationStateCount);
		std::vector<sAnimationState> animationStates;
	};
	/**
	Held per mesh.
	Used as such:
	vector<sMJHolder> thing;
	thing.resize(mTempMeshList.size());
	thing[0].jointList.resize(mTempMeshList[0].jointList.size());
	**/
	struct sMJHolder
	{
		std::vector<sJoint> jointList; // .resize(mesh[0].jointCount)
		std::vector<sJHolder> perJoint;
	};

	struct  sMaterial
	{
		char materialName[256];

		float ambientColor[3];
		float diffuseColor[3];
		float specularColor[3];

		float shinyFactor;

		char diffuseTexture[256];
		char specularTexture[256];
		char normalTexture[256];
	};

	struct  sLight
	{
		int lightID;

		float lightPos[3];
		float lightRot[3];
		float lightScale[3];

		float color[3];
		float intensity;
	};

	struct  sCamera
	{
		float camPos[3];
		float upVector[3];

		float fieldOfView;
		float nearPlane;
		float farPlane;
	};

	class MoleReader {
	public:
		void readFromBinary(const char* filePath);
		/**
		Gets a vector containing the entire list of meshes.
		**/
		const std::vector<sMesh> getMeshList();
		/**
		Gets a vector containing vectors of mesh-child-indices of meshes.
		**/
		const std::vector<sMChildHolder> getMeshChildList();
		/**
		Gets a vector containing the entire list of materials.
		**/
		const std::vector< sMaterial> getMaterialList();
		/**
		Gets a vector containing the entire list of cameras.
		**/
		const std::vector<sCamera> getCameraList();
		/**
		Gets a vector containing the entire list of lights.
		**/
		const std::vector<sLight> getLightList();
		/**
		Gets a vector containing vectors of joints and "joint related stuff".
		Inside the "joint related stuff" are vectors containing meshChildren
		and animationStates. Animationstates are what separates animations from
		one another for each joint.
		**/
		const std::vector<sMJHolder> getJointKeyList();

		const sMainHeader getMainHeader();

		/**
		Get the index of the mesh with the queried name.
		If no matching mesh is found, -1337 is returned.
		Use this function sparingly, it's algorithm loops
		through ALL of the meshi and compare their names.
		**/
		const int getMeshIndex(std::string meshName);
		/**
		Gets the mesh at the given meshIndex.
		Usage of this mesh is neccessary for getting
		a bunch of mesh-related data.
		Examples are:
		JointCount
		skelVertexCount
		vertexCount
		isAnimated
		**/
		const sMesh getMesh(int meshIndex);
		/**
		Gets the vector containing keyframes for an animationstate in a joint in a mesh.
		Animationstates are "groups" that encapsulate keyFrames. Each animationstate represents
		a different animation. Examples of different animationstates are "walking cycle",
		"attack animation", "idle animation" etc.
		**/
		const std::vector<sKeyFrame> getKeyList(int meshIndex, int jointIndex, int animationState);
		/**
		Gets the vector containing indexes to meshChildren belonging to a mesh.
		**/
		const std::vector<sMeshChild> getMeshChildList(int meshIndex);

		const sMaterial getMaterial(int materialIndex);
		/**
		Only useful if mesh has isAnimated == true
		**/
		const sJoint getJoint(int meshIndex, int jointIndex);
		/**
		Gets the vector containing indexes to meshChildren belonging to
		a joint.
		**/
		const std::vector<sMeshChild> getJointMeshChildList(int meshIndex, int jointIndex);
		/**
		Gets the vector containing skeletal vertices belonging to a mesh.
		What separates a skeletal vertex from a regular vertex is that a
		skeletal vertex has influences and weights.
		An influence is an index to the joint affecting the skeletal vertex.
		A weight is how high of a "percentage" of the joint's transform affects
		the skeletal vertex, expressed in numbers between 0 and 1.
		Our skeletal vertices will have a maximum of 4 influences and 4 weights.
		The sum of the weights must always be 1, making it "100%".
		**/
		const std::vector<sSkelAnimVertex> getSkelVertexList(int meshIndex);
		/**
		Gets the vector containing vertices belonging to a mesh.
		**/
		const std::vector<sVertex> getVertexList(int meshIndex);

		MoleReader();
		~MoleReader();

	private:

		int prevMeshes;
		int prevCameras;
		int prevLights;
		int prevMaterials;
		int prevJoints;


		//Vectors holding all of the data.
		std::vector<sMesh> pmRead_meshList;
		//Holds the meshChildren of meshes.
		std::vector<sMChildHolder> pmRead_meshChildList;
		std::vector<sMJHolder> pmRead_meshJointHolder;
		std::vector<sCamera> pmRead_cameraList;
		std::vector<sLight> pmRead_lightList;
		std::vector< sMaterial> pmRead_materialList;
		std::vector<m> pmRead_mList;
		std::vector<mk> pmRead_mkList;
		//std::vector<read_sJoint> pmRead_jointList;
		//std::vector<read_sKeyFrame> pmRead_KeyList;
		//std::vector<read_sAnimationStateTracker> pmRead_animSTrackerList;
		//std::vector<read_sAnimationState> pmRead_animStateList;

		//Struct objects
		sMainHeader pmRead_mainHeader;
		//read_sVertex gRead_vertex;
		//read_sMaterial gRead_materialData;
		//read_sLight gRead_lightData;
		//read_sCamera gRead_camData;
	};
