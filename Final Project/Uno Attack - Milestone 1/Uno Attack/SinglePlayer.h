#ifndef SINGLEPLAYER_H
#define SINGLEPLAYER_H

class SinglePlayer : public State
{
	public:
		SinglePlayer(DWORD id);

		virtual void Load();
		virtual void Close();
		virtual void OnLostDevice();
		virtual void OnResetDevice();
		virtual void Update(float dt);
		virtual void Draw();

	private:

};

#endif