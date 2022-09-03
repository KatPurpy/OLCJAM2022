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

	StateEnter  s_next_enter = nullptr;
	StateLeave  s_next_leave = nullptr;
	StateUpdate s_next_update = nullptr;
	StateDraw   s_next_draw = nullptr;
	static bool switchScheduled;

	void DoSwitch()
	{
		if(s_leave)
			s_leave();

		s_enter = s_next_enter;
		s_leave = s_next_leave;
		s_update = s_next_update;
		s_draw = s_next_draw;

		s_enter();

		switchScheduled = false;
		
	}
	template <typename T>
	void
	SwitchState()
	{
		switchScheduled = true;
		s_next_enter = T::Enter;
		s_next_leave = T::Leave;
		s_next_update = T::Update;
		s_next_draw = T::Draw;

	}

	void
	Update()
	{
		if(switchScheduled)
		{
			DoSwitch();
		}
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
