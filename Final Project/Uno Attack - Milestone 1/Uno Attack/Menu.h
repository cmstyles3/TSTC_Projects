#ifndef MENU_H
#define MENU_H

class Menu : public State
{
	public:
		Menu(DWORD id);

		virtual void Load();
		virtual void Close();
		virtual void OnLostDevice();
		virtual void OnResetDevice();
		virtual void Update(float dt);
		virtual void Draw();

	private:

};

#endif