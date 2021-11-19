#include "Camera.h"
#include "Window.h"
#include <algorithm>

void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1); // defining direction
	Vector3 right = rotation * Vector3(1, 0, 0);

	float speed = 30.0f * dt;
	
	if (isFollowingTrack) {
		position += (direction * dt);

		Vector3 po = position - track.front();
		if (po.Length() < 0.1f) {
			NextWaypoint();
		}
	}
	else {
		// modifying position variable to move camera
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position -= right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position += right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y += speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y -= speed;
		}
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_F)) {
		isFollowingTrack = !isFollowingTrack;
	}
}

/*
	Creating the view matreix required for the vertex shader.

	This is the INVERSE of the matrix created from the amera position and rotation values
	---> Now we could inverse, but its very expensive (even the GLSL call), so we can be sneaky and
		 just negate the pitch, yaw and position of the camra when forming the matrix, which does the
		 same as inverting the matrix, but is far cheaper!
*/
Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
}

void Camera::NextWaypoint() {
	track.push(track.front());
	track.pop();

	direction = (track.front() - position);
	direction.Normalise();
	direction = direction * cameraTrackSpeed;
}