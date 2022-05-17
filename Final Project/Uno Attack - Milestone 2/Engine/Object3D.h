// OBJECT3D.H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT3D_H
#define OBJECT3D_H


class Object3D
{
	public:
		Object3D();
		~Object3D();

		void Reset();

		void MoveOnX(float amount);
		void MoveOnY(float amount);
		void MoveOnZ(float amount);

		void Pitch(float angle);
		void   Yaw(float angle);
		void  Roll(float angle);

		D3DXVECTOR3 GetPosition();
		void        SetPosition(D3DXVECTOR3 position);

		D3DXVECTOR3 GetRight();
		D3DXVECTOR3 GetUp();
		D3DXVECTOR3 GetLook();

		void SetRight(D3DXVECTOR3 right);
		void SetUp(D3DXVECTOR3 up);
		void SetLook(D3DXVECTOR3 look);

		D3DXMATRIX GetMatrix();
		D3DXMATRIX GetViewMatrix();
	private:
		D3DXVECTOR3 m_right, m_up, m_look;
		D3DXVECTOR3 m_position;
};


#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////