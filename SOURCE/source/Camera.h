#ifndef CAMERA_H
#define CAMERA_H

namespace dae
{
	struct Matrix;
	struct Vector2;
	struct Vector3;
	struct Vector4;

	class Camera final
	{
	public:

		explicit Camera(const Vector3& origin, float fovAngle, float aspectRatio, float zn, float zf);
		~Camera() = default;

		Camera(const Camera&) = delete;
		Camera(Camera&&) noexcept = delete;
		Camera& operator=(const Camera&) = delete;
		Camera& operator=(Camera&&) noexcept = delete;

		void Update(const Timer* const pTimer);

		const Vector3& GetOrigin() const;

		const Vector3& GetForwardVector() const;
		const Vector3& GetUpVector() const;
		const Vector3& GetRightVector() const;

		const Matrix& GetViewMatrix() const;
		const Matrix& GetProjectionMatrix() const;

	private:

		const float m_AspectRatio;

		Vector3 m_Origin;
		float m_FovAngle;
		float m_FovValue;

		Vector3 m_Forward{ Vector3::UnitZ };
		Vector3 m_Up{ Vector3::UnitY };
		Vector3 m_Right{ Vector3::UnitX };

		float m_TotalPitch;
		float m_TotalYaw;

		Matrix m_InvViewMatrix;
		Matrix m_ViewMatrix;
		Matrix m_ProjectionMatrix;

		const float m_Zfar;
		const float m_Znear;
		
		void CalculateViewMatrix();
		void CalculateProjectionMatrix();

	};
}

#endif // !CAMERA_H