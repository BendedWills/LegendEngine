#include <iostream>
#include <chrono>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
		+ std::to_string(vec.z) + "Z"
		+ std::to_string(vec.w) + "W";
}

void PrintMat4x4(const Matrix4x4f& mat)
{
	std::cout << "[" << mat.c[0].x << ", " << mat.c[1].x << ", " << mat.c[2].x 
		<< ", " << mat.c[3].x << "]" << std::endl;
	std::cout << "[" << mat.c[0].y << ", " << mat.c[1].y << ", " << mat.c[2].y
		<< ", " << mat.c[3].y << "]" << std::endl;
	std::cout << "[" << mat.c[0].z << ", " << mat.c[1].z << ", " << mat.c[2].z 
		<< ", " << mat.c[3].z << "]" << std::endl;
	std::cout << "[" << mat.c[0].w << ", " << mat.c[1].w << ", " << mat.c[2].w 
		<< ", " << mat.c[3].w << "]" << std::endl;
}

void NormalizeVector(Vector3f* pVec)
{

}

int main()
{
	// Adding vectors
	std::cout << "Adding vectors: \n" << std::endl;
	{
		Vector2f vec1(1.0f, 1.0f);
		Vector2f vec2(2.0f, 2.0f);
		Vector2f added = vec1 + vec2;

		std::cout << vec1[0] << std::endl;

		std::cout << "First vector: " << Vec2Str(vec1) << std::endl;
		std::cout << "Second vector: " << Vec2Str(vec2) << std::endl;
		std::cout << "Added vector: " << Vec2Str(added) << std::endl;
	}
	std::cout << "\n" << std::endl;

	// Adding vectors
	std::cout << "Vector normalization: \n" << std::endl;
	{
		Vector3f vec(1, 0, 0);
		glm::vec3 vec2(vec.x, vec.y, vec.z);

		Vector3f normalized = Vector3f::Normalize(vec);
		glm::vec3 expected = glm::normalize(vec2);

		std::cout << "Original vector: " << Vec3Str(vec) << std::endl;
		std::cout << "Normalized: " << Vec3Str(normalized) << std::endl;
		std::cout << "Expected: " << 
			Vec3Str(Vector3f(expected.x, expected.y, expected.z)) << std::endl;
	}
	std::cout << "\n" << std::endl;

	// Multiplying matrices
	std::cout << "Multiplying matrices: \n" << std::endl;
	{
		{
			Matrix4x4f mat1 = Matrix4x4f::MakeIdentity();
			PrintMat4x4(mat1);

			std::cout << "*" << std::endl;

			Matrix4x4f mat2;
			mat2[0] = Vector4f(0, 0, 0, 0);
			mat2[1] = Vector4f(0, 0, 0, 0);
			mat2[2] = Vector4f(0, 0, 0, 0);
			mat2[3] = Vector4f(1, 2, 3, 1);
			PrintMat4x4(mat2);

			std::cout << "=" << std::endl;

			Matrix4x4f mat3 = mat1 * mat2;
			PrintMat4x4(mat3);

		}
		std::cout << "\n" << std::endl;
		
		{
			Matrix4x4f mat1 = Matrix4x4f::MakeIdentity();
			mat1 *= 10;

			PrintMat4x4(mat1);

			std::cout << "*" << std::endl;

			Matrix4x4f mat2;
			mat2[0] = Vector4f(0, 0, 0, 0);
			mat2[1] = Vector4f(0, 0, 0, 0);
			mat2[2] = Vector4f(0, 0, 0, 0);
			mat2[3] = Vector4f(1, 2, 3, 1);
			PrintMat4x4(mat2);

			std::cout << "=" << std::endl;

			Matrix4x4f mat3 = mat1 * mat2;
			PrintMat4x4(mat3);
		}
	}
	std::cout << "\n" << std::endl;

	// Matrix copying
	std::cout << "Matrix copying: \n" << std::endl;
	{
		Matrix4x4f mat1 = Matrix4x4f::MakeIdentity();
		Matrix4x4f mat2 = mat1;

		std::cout << "Original:" << std::endl;
		PrintMat4x4(mat1);
		std::cout << "Copied:" << std::endl;
		PrintMat4x4(mat2);
	}
	std::cout << "\n" << std::endl;

	glm::mat4 transform(1.0f);
	transform = glm::rotate(transform, 0.0f, glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, 0.0f, glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, 0.0f, glm::vec3(0, 0, 1));
	transform = glm::translate(transform, glm::vec3(0, 0, 0));
	transform = glm::scale(transform, glm::vec3(1, 1, 1));

	std::cout << "[" << transform[0].x << ", " << transform[1].x << ", " << transform[2].x
		<< ", " << transform[3].x << "]" << std::endl;
	std::cout << "[" << transform[0].y << ", " << transform[1].y << ", " << transform[2].y
		<< ", " << transform[3].y << "]" << std::endl;
	std::cout << "[" << transform[0].z << ", " << transform[1].z << ", " << transform[2].z
		<< ", " << transform[3].z << "]" << std::endl;
	std::cout << "[" << transform[0].w << ", " << transform[1].w << ", " << transform[2].w
		<< ", " << transform[3].w << "]" << std::endl;

	Matrix4x4f testMat = Matrix4x4f::MakeIdentity();
	testMat = Matrix4x4f::Rotate(testMat, 0.0f, Vector3f(1, 0, 0));
	testMat = Matrix4x4f::Rotate(testMat, 0.0f, Vector3f(0, 1, 0));
	testMat = Matrix4x4f::Rotate(testMat, 0.0f, Vector3f(0, 0, 1));
	testMat = Matrix4x4f::Translate(testMat, Vector3f(0, 0, 0));
	testMat = Matrix4x4f::Scale(testMat, Vector3f(1, 1, 1));

	std::cout << "Final:\n";
	std::cout << "[" << testMat.c[0].x << ", " << testMat.c[1].x << ", " << testMat.c[2].x
		<< ", " << testMat.c[3].x << "]" << std::endl;
	std::cout << "[" << testMat.c[0].y << ", " << testMat.c[1].y << ", " << testMat.c[2].y
		<< ", " << testMat.c[3].y << "]" << std::endl;
	std::cout << "[" << testMat.c[0].z << ", " << testMat.c[1].z << ", " << testMat.c[2].z
		<< ", " << testMat.c[3].z << "]" << std::endl;
	std::cout << "[" << testMat.c[0].w << ", " << testMat.c[1].w << ", " << testMat.c[2].w
		<< ", " << testMat.c[3].w << "]" << std::endl;

	return EXIT_SUCCESS;
}