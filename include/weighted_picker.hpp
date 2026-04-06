#pragma once
#include <random>

template<typename T>
class WeightedPicker {
public:
	void Add(T value, int weight) {
		m_weights.emplace_back(value, weight);
		m_totalWeight += weight;
	}

	T Pick(std::mt19937& rng) const {
		std::uniform_int_distribution<int> dist(1, m_totalWeight);
		const int roll = dist(rng);

		int cumulative = 0;
		for (const auto& [value, weight] : m_weights) {
			cumulative += weight;
			if (roll <= cumulative) return value;
		}
		return m_weights.back().first;
	}

private:
	std::vector<std::pair<T, int>> m_weights;
	int m_totalWeight = 0;
};
