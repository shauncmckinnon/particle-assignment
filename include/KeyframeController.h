#pragma once

#include <vector>

template <typename T>
class KeyframeController
{
private:
	unsigned int m_pCurrentKeyframe;
	unsigned int m_pNextKeyframe;
	std::vector<T> m_pKeys;
	float m_pKeyLocalTime;

public:
	float curvePlaySpeed;
	bool paused;
	bool doesSpeedControl;

	KeyframeController()
		: m_pCurrentKeyframe(0),
		m_pNextKeyframe(1),
		m_pKeyLocalTime(0.0f),
		paused(false),
		doesSpeedControl(false),
		curvePlaySpeed(1.0f)
	{

	}

	T update(float dt)
	{
		if (m_pKeys.size() < 2)
			return T();	

		// Spoilers for next week: you'll be implementing this function
		//if (doesSpeedControl)
		//	return speedControlledUpdate(dt);
		//else
			return noSpeedControlUpdate(dt);
	}

	void updateSegmentIndices()
	{
		if (m_pKeyLocalTime >= 1.0f)
		{
			m_pKeyLocalTime = 0.0f;
			m_pCurrentKeyframe++;
			m_pNextKeyframe++;

			// If we're at the end of the animation, jump back to beginning
			// Note: you can add additional logic here to handle end of animation behaviour
			// such as: ping-ponging (playing in reverse back to beginning), closed loop, etc.
			if (m_pNextKeyframe >= m_pKeys.size())
			{
				m_pCurrentKeyframe = 0;
				m_pNextKeyframe = 1;
			}
		}
	}

	// You'll be implementing this function next week
	T speedControlledUpdate(float dt)
	{
		return T();
	}

	T noSpeedControlUpdate(float dt)
	{
		m_pKeyLocalTime += dt;

		updateSegmentIndices();

		T p0 = m_pKeys[m_pCurrentKeyframe];
		T p1 = m_pKeys[m_pNextKeyframe];

		return Math::lerp<T>(p0, p1, m_pKeyLocalTime);
	}

	void addKey(T key)
	{
		m_pKeys.push_back(key);
	}

	void setKey(unsigned int idx, T key)
	{
		if (idx >= m_pKeys.size())
			return;
		m_pKeys[idx] = key;
	}
};