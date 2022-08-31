using StateEnter = void (*)();
using StateLeave = void (*)();
using StateUpdate = void (*)();
using StateDraw = void (*)();

namespace StateManagement
{
	StateEnter s_enter = nullptr;
	StateLeave s_leave = nullptr;
	StateUpdate s_update = nullptr;
	StateDraw s_draw = nullptr;
	template <typename T>
	void
	SwitchState()
	{
		if(s_leave)
			s_leave();

		s_enter = T::Enter;
		s_leave = T::Leave;
		s_update = T::Update;
		s_draw = T::Draw;

		s_enter();
	}

	void
	Update()
	{
		assert(s_update);
		s_update();
	}

	void
	Draw()
	{
		assert(s_draw);
		s_draw();
	}

	void
	Leave()
	{
		assert(s_leave);
		s_leave();
	}
} // namespace StateManagement
