#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;

	// We're keeping the track width of 15 units
	// and the offset by 5 units 
	trackHalfWidth = 15.0f;
	offset = 5.0f;

}

CCatmullRom::~CCatmullRom()
{

}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{

	glm::vec3 a = p1;
	glm::vec3 b = (-p0 + p2) * 0.5f;
	glm::vec3 c = (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3) * 0.5f;
	glm::vec3 d = (-p0 + 3.0f*p1 - 3.0f*p2 + p3) * 0.5f;

	return a + (b*t) + (c*t*t) + (d*t*t*t);

}


void CCatmullRom::SetControlPoints(int num)
{
	// Set control points (m_controlPoints) here, or load from disk
	switch (num)
	{
		// MAIN PATH
	case 0:
		m_controlPoints.push_back(glm::vec3(0, 100, 0));
		m_controlPoints.push_back(glm::vec3(100, 100, 0));
		m_controlPoints.push_back(glm::vec3(200, 150, 0));
		m_controlPoints.push_back(glm::vec3(300, 200, 0));
		m_controlPoints.push_back(glm::vec3(400, 200, 0));
		m_controlPoints.push_back(glm::vec3(400, 200, 100));
		m_controlPoints.push_back(glm::vec3(400, 200, 200));
		m_controlPoints.push_back(glm::vec3(500, 200, 200));
		m_controlPoints.push_back(glm::vec3(600, 200, 200));
		m_controlPoints.push_back(glm::vec3(600, 200, 300));
		m_controlPoints.push_back(glm::vec3(600, 200, 300));
		m_controlPoints.push_back(glm::vec3(600, 200, 400));
		m_controlPoints.push_back(glm::vec3(600, 250, 600));
		m_controlPoints.push_back(glm::vec3(600, 300, 800));
		m_controlPoints.push_back(glm::vec3(600, 300, 900));
		m_controlPoints.push_back(glm::vec3(550, 300, 900));
		m_controlPoints.push_back(glm::vec3(500, 300, 900));
		m_controlPoints.push_back(glm::vec3(400, 300, 900));
		m_controlPoints.push_back(glm::vec3(300, 300, 800));
		m_controlPoints.push_back(glm::vec3(200, 300, 900));
		m_controlPoints.push_back(glm::vec3(100, 300, 800));
		m_controlPoints.push_back(glm::vec3(0, 300, 900));
		m_controlPoints.push_back(glm::vec3(-100, 300, 800));
		m_controlPoints.push_back(glm::vec3(-200, 250, 800));
		m_controlPoints.push_back(glm::vec3(-300, 200, 800));
		m_controlPoints.push_back(glm::vec3(-300, 200, 700));
		m_controlPoints.push_back(glm::vec3(-300, 200, 600));
		m_controlPoints.push_back(glm::vec3(-400, 200, 500));
		m_controlPoints.push_back(glm::vec3(-300, 200, 400));
		m_controlPoints.push_back(glm::vec3(-300, 150, 200));
		m_controlPoints.push_back(glm::vec3(-300, 100, 0));
		m_controlPoints.push_back(glm::vec3(-200, 100, 0));
		m_controlPoints.push_back(glm::vec3(-100, 100, 0));
		break;

		// INTRO PATH
	case 1:
		m_controlPoints.push_back(glm::vec3(-1000, 300, 0));
		m_controlPoints.push_back(glm::vec3(-900, 280, 50));
		m_controlPoints.push_back(glm::vec3(-800, 260, 100));
		m_controlPoints.push_back(glm::vec3(-700, 240, 50));
		m_controlPoints.push_back(glm::vec3(-600, 220, 0));
		m_controlPoints.push_back(glm::vec3(-500, 200, -50));
		m_controlPoints.push_back(glm::vec3(-400, 180, -100));
		m_controlPoints.push_back(glm::vec3(-300, 160, -50));
		m_controlPoints.push_back(glm::vec3(-200, 140, 0));
		m_controlPoints.push_back(glm::vec3(-100, 120, 0));
		m_controlPoints.push_back(glm::vec3(0, 100, 0));
		m_controlPoints.push_back(glm::vec3(0, 95, 0));


		FinalControlLinePoint = m_controlPoints.back();

		break;
	}

	for (int i = 0; i < m_controlPoints.size(); i++)
	{
		glm::vec3 T = glm::normalize(m_controlPoints[(i + 1) % m_controlPoints.size()] - m_controlPoints[i]);
		glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 B = glm::normalize(glm::cross(N, T));
		m_controlUpVectors.push_back(B);

	}

}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();

	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline(int num)
{
	// Call Set Control Points
	SetControlPoints(num);

	// Call UniformlySampleControlPoints with the number of samples required
	switch (num)
	{
	case 0:
		UniformlySampleControlPoints(2100);
		break;
	case 1:
		UniformlySampleControlPoints(700);
		break;
	}


}


void CCatmullRom::CreateOffsetCurves()
{
	// Current and Next Point
	glm::vec3 p;
	glm::vec3 pNext;

	// Tangent
	glm::vec3 T;

	// Normal
	glm::vec3 N;

	// Binormal
	glm::vec3 B;

	// Left Edge and Lane Point
	glm::vec3 edge_l;
	glm::vec3 lane_l;

	// Right Edge and Lane Point
	glm::vec3 edge_r;
	glm::vec3 lane_r;

	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		// Using the modular to stay within vector range
		pNext = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()];

		p = m_centrelinePoints[i];

		// Distance between next point and current point
		T = glm::normalize(pNext - p);

		// Cross Product of the y axis in world co-ordinates
		// and the tangent vector
		N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0, 0.0f)));

		// Cross product of the Normal and the tangent vector
		// to get the true normal of the point
		B = glm::normalize(glm::cross(N, T));

		// Get the point from the centre
		// on either sides along the normal
		// by half width 
		edge_l = p - trackHalfWidth * N;
		lane_l = p - (trackHalfWidth - offset) * N;

		// And push them to the respective vectors
		m_leftEdgePoints.push_back(edge_l);
		m_leftLanePoints.push_back(lane_l);

		edge_r = p + trackHalfWidth * N;
		lane_r = p + (trackHalfWidth - offset) * N;

		m_rightEdgePoints.push_back(edge_r);
		m_rightLanePoints.push_back(lane_r);

	}

}


void CCatmullRom::CreateTrack(string fileName)
{
	// Adding points like shown below
	// Track is moving upward

	//3----------4
	// \
	//  \
	//   \
	//    \
	//     \
	//      \
	//       \
	//        \ 
	//         \
	//          \
	//1----------2

	// Left Edge and Right Edge path vectors
	// will have the same size so we can use any one
	for (int i = 0; i <= m_leftEdgePoints.size(); i++)
	{
		m_trackPoints.push_back(m_leftEdgePoints[i % m_leftEdgePoints.size()]);
		m_trackPoints.push_back(m_rightEdgePoints[i % m_rightEdgePoints.size()]);
	}

	// Adding the up vectors for each track point
	for (int i = 0; i <= m_trackPoints.size(); i++)
	{
		// We're doing this because the left points
		// are at even places
		if (i % 2 == 0)
		{
			m_trackUpVectors.push_back(GetNormal(m_trackPoints[i % m_trackPoints.size()], (int)LeftEdge));
		}
		else
		{
			m_trackUpVectors.push_back(GetNormal(m_trackPoints[i % m_trackPoints.size()], (int)RightEdge));
		}

	}

	// Loading in the textures and setting
	// the samplers
	m_texture.Load(fileName);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	//Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	// Adding the texture co-ordinates
	// the same way we added the track points
	vector<glm::vec2> texCoords;
	texCoords.push_back(glm::vec2(0.0f, 0.0f));
	texCoords.push_back(glm::vec2(1.0f, 0.0f));
	texCoords.push_back(glm::vec2(0.0f, 1.0f));
	texCoords.push_back(glm::vec2(1.0f, 1.0f));

	// Add the position, texture coords and
	// the normal for each point
	for (unsigned int i = 0; i < m_trackPoints.size(); i++)
	{
		vbo.AddData(&m_trackPoints[i], sizeof(glm::vec3));
		vbo.AddData(&texCoords[i % 4], sizeof(glm::vec2));
		vbo.AddData(&m_trackUpVectors[i], sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));

	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}


void CCatmullRom::RenderTrack()
{
	// Disabling cull face to
	// make the track at higher
	// levels to render normally
	glBindVertexArray(m_vaoTrack);

	glDisable(GL_CULL_FACE);

	m_texture.Bind();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_trackPoints.size());

	glEnable(GL_CULL_FACE);

}

// Get how many laps have we completed
int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

// GET functions of each vector paths

vector<glm::vec3> CCatmullRom::GetCentreLinePoints()
{
	return m_centrelinePoints;
}

vector<glm::vec3> CCatmullRom::GetLeftEdgePoints()
{
	return m_leftEdgePoints;
}

vector<glm::vec3> CCatmullRom::GetRightEdgePoints()
{
	return m_rightEdgePoints;
}

// GET the floating values

float CCatmullRom::GetTrackHalfWidth()
{
	return trackHalfWidth;
}
float CCatmullRom::GetOffset()
{
	return offset;
}

// Get the point from the requested lane and position
glm::vec3 CCatmullRom::GetPoint(int &lane, int index, bool inAir)
{

	switch ((Lane)lane)
	{
	case Left:
		if (inAir == true)
		{
			return m_leftLanePoints[index] + (10.0f * GetNormal(m_rightLanePoints[index], lane));
		}
		else
		{
			return m_leftLanePoints[index];
		}

		break;
	case Center:
		if (inAir == true)
		{
			return m_centrelinePoints[index] + (10.0f * GetNormal(m_rightLanePoints[index], lane));
		}
		else
		{
			return m_centrelinePoints[index];
		}
		break;
	case Right:
		if (inAir == true)
		{
			return m_rightLanePoints[index] + (10.0f * GetNormal(m_rightLanePoints[index], lane));
		}
		else
		{
			return m_rightLanePoints[index];
		}
		break;
	}

	return glm::vec3(0.0f);

}

glm::vec3 GetNearestPointFromLane(vector<glm::vec3> lane_points, glm::vec3 point)
{

	// Using an iterator to loop through the vector
	vector<glm::vec3>::iterator it = lane_points.begin();

	// Move the pointer to the desired address
	it = std::find(lane_points.begin(), lane_points.end(), point);

	// By calculating the distance, we can
	// determine the index
	int i = std::distance(lane_points.begin(), it);

	// Return the next point without going out of range
	return lane_points[(i + 1) % lane_points.size()];

}

// Purpose of this method is to use the right path vector
// to calculate its nearest point
glm::vec3 CCatmullRom::GetNearestPoint(glm::vec3 point, int lane)
{

	switch (Lane(lane))
	{
	case LeftEdge:
	{
		return GetNearestPointFromLane(m_leftEdgePoints, point);
		break;
	}
	case Left:
	{
		return GetNearestPointFromLane(m_leftLanePoints, point);
		break;
	}
	case Center:
	{
		return GetNearestPointFromLane(m_centrelinePoints, point);
		break;
	}
	case Right:
	{
		return GetNearestPointFromLane(m_rightLanePoints, point);
		break;
	}
	case RightEdge:
	{
		return GetNearestPointFromLane(m_rightEdgePoints, point);
		break;
	}

	}

	return glm::vec3(0.0f);
}


// Both methods do the same action but return different
// values. One returns the TNB frame and the other,
// returns the axis
glm::mat4 CCatmullRom::GetRotationMatrix(glm::vec3 point, int lane)
{

	glm::vec3 T = glm::normalize(point - GetNearestPoint(point, lane));
	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	return glm::mat4(glm::mat3(T, B, N));

}

glm::vec3 CCatmullRom::GetNormal(glm::vec3 point, int lane)
{
	glm::vec3 T = glm::normalize(point - GetNearestPoint(point, lane));
	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	return B;
}