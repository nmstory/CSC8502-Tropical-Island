#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include <queue>

class Camera {
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
		isFollowingTrack = false;
		time = 0;
	}

	Camera(float pitch, float yaw, Vector3 position, std::queue<Vector3> track) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		this->track = track;
		if (track.size() == 0) {
			isFollowingTrack = false;
		}
		else {
			isFollowingTrack = true;
			result = (track.front() - position);
			result.Normalise();
			result = result * cameraTrackSpeed; // speed
		}
		
		time = 0;
	}

	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);
	Matrix4 BuildViewMatrix();

	Vector3   GetPosition()  const { return  position; }
	void      SetPosition(Vector3  val) { position = val; }

	float     GetYaw()    const { return  yaw; }
	void      SetYaw(float y) { yaw = y; }

	float     GetPitch()  const { return  pitch; }
	void      SetPitch(float p) { pitch = p; }

protected:
	float yaw;
	float pitch;
	Vector3 position;

	float time;
	Vector3 previousPos;
	bool isFollowingTrack;
	std::queue<Vector3> track; // Camera track of waypoints
	float cameraTrackSpeed = 50.0f;

	Vector3 result;
	
};