#pragma once

#include <random>

/*
 * A simple weighted random picker. 
 * Add items with their weights, and then call Pick() to get a random item based on the weights.
 */
template<typename T>
class WeightedPicker {
public:
	// Add an item with its weight. Higher weight means higher chance of being picked.
	void Add(T value, int weight) {
		m_weights.emplace_back(value, weight);
		m_totalWeight += weight;
	}

	// Pick a random item based on the weights. Requires a random number generator.
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
