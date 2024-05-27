#ifndef DATATYPES_H
#define DATATYPES_H

namespace dae
{
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct ColorRGB;

	enum class FilteringMode
	{
		Point = 0,
		Linear,
		Anisotropic,
	};

	struct Vertex
	{
		Vector3 position;
		Vector2 uv;
		Vector3 normal;
		Vector3 tangent;
	};
}

#endif // !DATATYPES_H
