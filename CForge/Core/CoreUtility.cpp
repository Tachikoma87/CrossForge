#include "CoreUtility.hpp"

namespace CForge {

	uint64_t CoreUtility::m_RndState = 88172645463325252ull;

	uint64_t CoreUtility::rand(void){
		m_RndState ^= m_RndState << 13;
		m_RndState ^= m_RndState >> 7;
		m_RndState ^= m_RndState << 17;
		return m_RndState/2ull;
	}//rand

	void CoreUtility::randSeed(uint64_t Seed) {
		m_RndState = Seed;
	}//randSeed
}