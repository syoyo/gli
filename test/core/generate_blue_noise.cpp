#include <cstdint>
#include <vector>
#include <random>
#include <array>
#include "glm/glm.hpp"
#include "gli/gli.hpp"

inline float toroidal_distance(gli::extent2d const& a, gli::extent2d const& b, int ImageWidth)
{
	// this returns the toroidal distance between the points
	// aka the interval [0, width) wraps around
	glm::vec2 d = glm::abs(glm::vec2(b) - glm::vec2(a));

	if (d.x > float(ImageWidth >> 1))
		d.x = float(ImageWidth) - d.x;

	if (d.y > float(ImageWidth >> 1))
		d.y = float(ImageWidth) - d.y;

	// returning squared distance cause why not
	return glm::dot(d, d);
}

int main(int argc, char** argv)
{
	size_t const ImageSize = 256;
	size_t const SampleCount = 256;

	std::random_device Random;
	std::mt19937 NumberGenerator(Random());
	std::uniform_int_distribution<int> UniformSampler(0, ImageSize - 1);

	gli::texture2d Samples(gli::FORMAT_RGBA8_UNORM_PACK8, gli::extent2d(ImageSize), 1);
	Samples.clear(glm::u8vec4(127, 127, 127, 255));

	std::vector<gli::extent2d> SamplePositions;

	for (size_t SampleIndex = 0; SampleIndex < SampleCount; ++SampleIndex)
	{
		printf("\rGenerating Blue Noise: %2.0f", float(SampleIndex) / float(SampleCount) * 100.f);

		float BestDistance = 0.0f;
		gli::extent2d BestCandidate(0);
		for (size_t CandidateIndex = 0, CandidateCount = SamplePositions.size(); CandidateIndex < CandidateCount; ++CandidateIndex)
		{
			gli::extent2d const CandidatePosition(
				UniformSampler(NumberGenerator),
				UniformSampler(NumberGenerator));

			float MinDistance = std::numeric_limits<float>::max();
			for (std::size_t i = 0, n = SamplePositions.size(); i < n; ++i)
			{
				float Distance = toroidal_distance(CandidatePosition, SamplePositions[i], ImageSize);
				if (Distance < MinDistance)
					MinDistance = Distance;
			}

			if (MinDistance > BestDistance)
			{
				BestDistance = MinDistance;
				BestCandidate = CandidatePosition;
			}
		}
		SamplePositions.push_back(BestCandidate);

		static const glm::u8vec4 Colors[] =
		{
			glm::u8vec4(255),
			glm::u8vec4(255, 0, 0, 255),
			glm::u8vec4(255, 255, 0, 255),
			glm::u8vec4(0, 255, 0, 255),
			glm::u8vec4(0, 255, 255, 255),
			glm::u8vec4(0, 0, 255, 255),
			glm::u8vec4(255, 0, 255, 255)
		};

		Samples.store<glm::u8vec4>(BestCandidate, 0, Colors[SampleIndex * 7 / SampleCount]);
	}

	gli::save(Samples, "BlueNoise.dds");

	return 0;
}

