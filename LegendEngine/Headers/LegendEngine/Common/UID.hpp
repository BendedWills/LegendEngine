#pragma once

#include <cstdint>

namespace le
{
	class UID
	{
	public:
		UID();

		explicit operator uint64_t() const;
		bool operator==(const UID& rhs) const;

		uint64_t Get() const;
	private:
		uint64_t m_UID;
	};
}