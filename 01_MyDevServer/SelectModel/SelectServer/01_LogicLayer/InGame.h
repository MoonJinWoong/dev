#pragma once


// ���⼭���� �սô� 
// TODO
// �ΰ��� Ŭ���� ���� --> Room ���� --> �κ� ���� --> �κ�Ŵ��� ���� 


namespace LogicLayer
{
	enum class GameState
	{
		NONE,
		START,
		ING,
		END
	};

	class InGame
	{
	public:
		InGame();
		~InGame();
		void Init();
	};
}