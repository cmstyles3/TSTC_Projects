#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

class MultiPlayer : public State
{
	public:
		MultiPlayer(DWORD id);

		virtual void Load();
		virtual void Close();
		virtual void OnLostDevice();
		virtual void OnResetDevice();
		virtual void Update(float dt);
		virtual void Draw();

	private:
};

#endif