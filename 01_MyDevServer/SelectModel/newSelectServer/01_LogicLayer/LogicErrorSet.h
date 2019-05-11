#pragma once


namespace LogicLayer
{
	enum class LOGIC_ERROR_SET : short
	{
		NONE = 0,
		UNASSIGNED_ERROR = 201,
		MAIN_INIT_NETWORK_INIT_FAIL = 206,
		USER_ID_DUPLICATE = 207,
		MAX_CLIENT_MANAGER = 208
	};
}