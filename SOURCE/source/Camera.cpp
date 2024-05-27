#include "pch.h"
#include "Camera.h"
#include "Matrix.h"

namespace dae
{
	Camera::Camera(const Vector3& origin, float fovAngle, float aspectRatio, float zn, float zf)
		: m_Origin{ origin }
		, m_FovAngle{ fovAngle }
		, m_Zfar{ zf }
		, m_Znear{ zn }
		, m_AspectRatio{ aspectRatio }
	{
		m_FovValue = tanf((m_FovAngle * TO_RADIANS) / 2.f);

		m_TotalPitch = 0.f;
		m_TotalYaw = 0.f;

		CalculateViewMatrix();
		CalculateProjectionMatrix();
	}

	void Camera::Update(const Timer* const pTimer)
	{
		// Camera Logics //
		constexpr int fovMin{ 0 };
		constexpr int fovMax{ 180 };
		constexpr float walkMovementSpeed{ 30.f };
		constexpr float sprintMovementSpeed{ 50.f };
		constexpr float sensitivity{ 0.2f };

		// Mouse Input //
		int mouseX;
		int mouseY;
		const uint32_t mouseState{ SDL_GetRelativeMouseState(&mouseX, &mouseY) };

		if (mouseState)
		{
			// MOUSE
			if (mouseState & SDL_BUTTON_RMASK && mouseState & SDL_BUTTON_LMASK)
			{
				m_Origin += -mouseY * sensitivity * Vector3::UnitY;
			}
			else if (mouseState & SDL_BUTTON_RMASK)
			{
				m_TotalPitch += -mouseY * sensitivity;
				m_TotalYaw += mouseX * sensitivity;
			}
			else if (mouseState & SDL_BUTTON_LMASK)
			{
				m_Origin += -mouseY * sensitivity * Vector3::UnitZ;
				m_TotalYaw += mouseX * sensitivity;
			}
		}

		// Keyboard Input //
		int nrOfKeys;
		bool isKeyPressed{};
		const uint8_t* pKeyboardState{ SDL_GetKeyboardState(&nrOfKeys) };
		for (int idx = 0; idx < nrOfKeys; idx++)
		{
			if (pKeyboardState[idx])
			{
				isKeyPressed = true;
				break;
			}
		}

		if (isKeyPressed)
		{
			float deltaTimeSpeed{};

			if (pKeyboardState[SDL_SCANCODE_LSHIFT])
			{
				deltaTimeSpeed = pTimer->GetElapsed() * sprintMovementSpeed;
			}
			else
			{
				deltaTimeSpeed = pTimer->GetElapsed() * walkMovementSpeed;
			}

			// WASD
			if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])	// forward
			{
				m_Origin += deltaTimeSpeed * m_Forward;
			}
			if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN]) // backwards
			{
				m_Origin -= deltaTimeSpeed * m_Forward;
			}
			if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT]) // left
			{
				m_Origin -= deltaTimeSpeed * m_Right;
			}
			if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT]) // right
			{
				m_Origin += deltaTimeSpeed * m_Right;
			}

			// Up and Down
			if (pKeyboardState[SDL_SCANCODE_SPACE] || pKeyboardState[SDL_SCANCODE_E]) // up
			{
				m_Origin += deltaTimeSpeed * Vector3::UnitY;
			}
			if (pKeyboardState[SDL_SCANCODE_LCTRL] || pKeyboardState[SDL_SCANCODE_Q]) // down
			{
				m_Origin += deltaTimeSpeed * -Vector3::UnitY;
			}

			//Change POV
			if (pKeyboardState[SDL_SCANCODE_T])	// pov down
			{
				m_FovAngle -= 1;

				if (m_FovAngle < fovMin) m_FovAngle = fovMin;

				m_FovValue = tanf((m_FovAngle * TO_RADIANS) * 0.5f);
			}
			else if (pKeyboardState[SDL_SCANCODE_G])	// pov up
			{
				m_FovAngle += 1;

				if (m_FovAngle > fovMax) m_FovAngle = fovMax;

				m_FovValue = tanf((m_FovAngle * TO_RADIANS) * 0.5f);
			}
			else if (pKeyboardState[SDL_SCANCODE_R])	// reset pov
			{
				m_FovAngle = 45;
				m_FovValue = tanf((m_FovAngle * TO_RADIANS) * 0.5f);
			}
		}

		if (isKeyPressed || mouseState)
		{
			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}
	}

	const Vector3& Camera::GetOrigin() const
	{
		return m_Origin;
	}

	const Vector3& Camera::GetForwardVector() const
	{
		return m_Forward;
	}

	const Vector3& Camera::GetUpVector() const
	{
		return m_Up;
	}

	const Vector3& Camera::GetRightVector() const
	{
		return m_Right;
	}

	const Matrix& Camera::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	const Matrix& Camera::GetProjectionMatrix() const
	{
		return m_ProjectionMatrix;
	}

	void Camera::CalculateViewMatrix()
	{
		// calculate view matrix
		m_InvViewMatrix = Matrix::CreateRotation(m_TotalPitch * TO_RADIANS, m_TotalYaw * TO_RADIANS, 0.f) * Matrix::CreateTranslation(m_Origin);
		m_ViewMatrix = m_InvViewMatrix.Inverse();

		m_Forward = m_ViewMatrix.TransformVector(-Vector3::UnitZ).Normalized();
		m_Forward.z *= -1.f;

		m_Right = { Vector3::Cross(Vector3::UnitY, m_Forward).Normalized() };
		m_Up = { Vector3::Cross(m_Forward, m_Right).Normalized() };
	}

	void Camera::CalculateProjectionMatrix()
	{
		// calculate Projection Matrix
		m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_FovValue, m_AspectRatio, m_Znear, m_Zfar);
	}
}