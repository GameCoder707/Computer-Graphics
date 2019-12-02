#pragma once

#include "Common.h"
#include "vertexBufferObject.h"
#include "vertexBufferObjectIndexed.h"
#include "Texture.h"


class CCatmullRom
{
	// Different Lanes of the Track
	enum Lane
	{
		LeftEdge = 0,
		Left = 1,
		Center = 2,
		Right = 3,
		RightEdge = 4
	};

public:
	CCatmullRom();
	~CCatmullRom();

	// Method to create the primary centreline path
	void CreateCentreline(int num);

	// Method to create the edges
	// of the centreline path
	void CreateOffsetCurves();

	// A method to create a track based
	// on the points provided
	void CreateTrack(string fileName);

	// We only need to render the track
	void RenderTrack();

	// A method used by track objects to get a point
	// from a 1) particular lane,
	//		  2) position, and
	//        3)  whether to place them in the air or ground
	glm::vec3 GetPoint(int &lane, int index, bool inAir);

	// This will return the rotation matrix of the requested point
	glm::mat4 GetRotationMatrix(glm::vec3 pos, int lane);

	// This will return the next point of the requested point
	glm::vec3 GetNearestPoint(glm::vec3 point, int lane);

	// This will return the normal axis of the requested point
	glm::vec3 GetNormal(glm::vec3 pos, int lane);

	// This is used specifically by the intro track
	// to stop looping and begin the game
	glm::vec3 FinalControlLinePoint;
	
	// Get functions of all the centreline paths
	vector<glm::vec3> GetCentreLinePoints();
	vector<glm::vec3> GetLeftEdgePoints();
	vector<glm::vec3> GetRightEdgePoints();

	// Return the currvent lap (starting from 0) based on distance along the control curve.
	int CurrentLap(float d); 

	// Return a point on the centreline based on a certain distance along the control curve.
	bool Sample(float d, glm::vec3 &p, glm::vec3 &up); 

	// Get the half width of the track
	float GetTrackHalfWidth();

	// Get how far the left and right lanes
	// should be pushed from the edges
	float GetOffset();

private:

	// Set the original control points of the path
	void SetControlPoints(int num);

	// Determine lengths along the control points, 
	// which is the set of control points forming the closed curve
	void ComputeLengthsAlongControlPoints();

	// Sample the points to form a smoothly curved path
	void UniformlySampleControlPoints(int numSamples);

	// Perform Catmull Rom spline interpolation between four
	// points, interpolating the space between p1 and p2
	glm::vec3 Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t);

	vector<float> m_distances;

	// Texture used on the track
	CTexture m_texture;

	// Half width of the track
	float trackHalfWidth;

	// Offset from the edges of the track
	float offset;

	// Vertex Array Objects for each path
	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoTrack;

	vector<glm::vec3> m_controlPoints;			// Control points, which are interpolated to produce the centreline points
	vector<glm::vec3> m_controlUpVectors;		// Control upvectors, which are interpolated to produce the centreline upvectors

	vector<glm::vec3> m_centrelinePoints;		// Centreline points
	vector<glm::vec3> m_centrelineUpVectors;	// Centreline upVectors

	vector<glm::vec3> m_leftEdgePoints;			// Left Edge curve points
	vector<glm::vec3> m_leftLanePoints;			// Left Lane curve points

	vector<glm::vec3> m_rightEdgePoints;		// Right Edge curve points
	vector<glm::vec3> m_rightLanePoints;		// Right Lane curve points

	vector <glm::vec3> m_trackPoints;			// Track Points
	vector <glm::vec3> m_trackUpVectors;		// Track Up Vectors

	unsigned int m_vertexCount;					// Number of vertices in the track VBO
};
