#include <iostream>
#include <chrono>
#include <thread>

#include <LegendEngine/LegendEngine.hpp>

using namespace LegendEngine;

std::string Vec2Str(const Vector2f& vec)
{
	return 
		std::to_string(vec.x) + "X, " 
		+ std::to_string(vec.y) + "Y";
}

std::string Vec3Str(const Vector3f& vec)
{
	return 
		std::to_string(vec.x) + "X, " 
		+ std::to_string(vec.y) + "Y, " 
		+ std::to_string(vec.z) + "Z";
}

std::string Vec4Str(const Vector4f& vec)
{
	return
		std::to_string(vec.x) + "X, "
		+ std::to_string(vec.y) + "Y, "
		+ std::to_string(vec.z) + "Z, "
		+ std::to_string(vec.w) + "W";
}

void PrintMat4x4(const Matrix4x4f& mat)
{
	std::cout << "[" << mat.c[0].x << ", " << mat.c[1].x << ", " << mat.c[2].x 
		<< ", " << mat.c[3].x << "]\n";
	std::cout << "[" << mat.c[0].y << ", " << mat.c[1].y << ", " << mat.c[2].y
		<< ", " << mat.c[3].y << "]\n";
	std::cout << "[" << mat.c[0].z << ", " << mat.c[1].z << ", " << mat.c[2].z
		<< ", " << mat.c[3].z << "]\n";
	std::cout << "[" << mat.c[0].w << ", " << mat.c[1].w << ", " << mat.c[2].w 
		<< ", " << mat.c[3].w << "]\n";
}

void NormalizeVector(Vector3f* pVec)
{

}

int main()
{
	// Adding vectors
	std::cout << "Adding vectors: \n\n";
	{
		Vector2f vec1(1.0f, 1.0f);
		Vector2f vec2(2.0f, 2.0f);
		Vector2f added = vec1 + vec2;

		std::cout << vec1[0] << "\n";

		std::cout << "First vector: " << Vec2Str(vec1) << "\n";
		std::cout << "Second vector: " << Vec2Str(vec2) << "\n";
		std::cout << "Added vector: " << Vec2Str(added) << "\n";
	}
	std::cout << "\n\n";

	// Adding vectors
	std::cout << "Vector normalization: \n\n";
	{
		Vector3f vec(1, 0, 0);
		Vector3f normalized = Vector3f::Normalize(vec);
		
		std::cout << "Original vector: " << Vec3Str(vec) << "\n";
		std::cout << "Normalized: " << Vec3Str(normalized) << "\n";
	}
	std::cout << "\n\n";

	// Multiplying matrices
	std::cout << "Multiplying matrices: \n\n";
	{
		{
			Matrix4x4f mat1 = Matrix4x4f::MakeIdentity();
			PrintMat4x4(mat1);

			std::cout << "*\n";

			Matrix4x4f mat2;
			mat2[0] = Vector4f(0, 0, 0, 0);
			mat2[1] = Vector4f(0, 0, 0, 0);
			mat2[2] = Vector4f(0, 0, 0, 0);
			mat2[3] = Vector4f(1, 2, 3, 1);
			PrintMat4x4(mat2);

			std::cout << "=\n";

			Matrix4x4f mat3 = mat1 * mat2;
			PrintMat4x4(mat3);

		}
		std::cout << "\n\n";
		
		{
			Matrix4x4f mat1 = Matrix4x4f::MakeIdentity();
			mat1 *= 10;

			PrintMat4x4(mat1);

			std::cout << "*\n";

			Matrix4x4f mat2;
			mat2[0] = Vector4f(0, 0, 0, 0);
			mat2[1] = Vector4f(0, 0, 0, 0);
			mat2[2] = Vector4f(0, 0, 0, 0);
			mat2[3] = Vector4f(1, 2, 3, 1);
			PrintMat4x4(mat2);

			std::cout << "=\n";

			Matrix4x4f mat3 = mat1 * mat2;
			PrintMat4x4(mat3);
		}
	}
	std::cout << "\n\n";

	// Matrix copying
	std::cout << "Matrix copying: \n\n";
	{
		Matrix4x4f mat1 = Matrix4x4f::MakeIdentity();
		Matrix4x4f mat2 = mat1;

		std::cout << "Original:\n";
		PrintMat4x4(mat1);
		std::cout << "Copied:\n";
		PrintMat4x4(mat2);
	}
	std::cout << "\n\n";

	// Quaternions
	std::cout << "Quaternions: \n\n";
	{
		Quaternion origin;

		std::cout << "Original:\n";
		std::cout
			<< origin.w << "W, "
			<< origin.x << "X, "
			<< origin.y << "Y, "
			<< origin.z << "Z"
		<< "\n";

		std::cout << "Euler angles:\n" << Vec3Str(origin.GetEulerAngles()) << "\n";
	}
	std::cout << "\n\n";

	std::cout << std::endl;

	return EXIT_SUCCESS;
}