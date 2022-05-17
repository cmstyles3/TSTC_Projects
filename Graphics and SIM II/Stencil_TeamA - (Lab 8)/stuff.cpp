	//D3DXCreateBox(Device, 2.0f, 2.0f, 0.5f, &Box, 0);
	//D3DXCreateBox(Device, 0.3f, 0.3f, 0.75f, &Box2, 0);

	//D3DXMATRIX temp, temp2;

	////Define Table -----------------------------------------------------------
	//Object[0].num_parts = 5;

	////Table Top
	//D3DXCreateCylinder(Device, 10.0f, 10.0f, 1.0f, 20, 4, &Object[0].mesh[0], 0);
	//D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	//D3DXMatrixTranslation(&temp2, 0.0f, 10.0f, 0.0f);
	//Object[0].local[0] = temp * temp2;
	//Object[0].mtrl[0] = BLUE_MTRL;

	////Table Legs
	//D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[1], 0);
	//D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	//D3DXMatrixTranslation(&temp2, 5.0f, 5.0f, 5.0f);
	//Object[0].local[1] = temp * temp2;
	//Object[0].mtrl[1] = BLUE_MTRL;

	//D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[2], 0);
	//D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	//D3DXMatrixTranslation(&temp2, -5.0f, 5.0f, 5.0f);
	//Object[0].local[2] = temp * temp2;
	//Object[0].mtrl[2] = BLUE_MTRL;

	//D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[3], 0);
	//D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	//D3DXMatrixTranslation(&temp2, -5.0f, 5.0f, -5.0f);
	//Object[0].local[3] = temp * temp2;
	//Object[0].mtrl[3] = BLUE_MTRL;

	//D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[4], 0);
	//D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	//D3DXMatrixTranslation(&temp2, 5.0f, 5.0f, -5.0f);
	//Object[0].local[4] = temp * temp2;
	//Object[0].mtrl[4] = BLUE_MTRL;
	////End Table ----------------------------------------------------

	////Define Chair
	//Object[1].num_parts = 6;

	//D3DXCreateBox(Device, 5.0f, 6.0f, 1.0f, &Object[1].mesh[0], 0);
	//D3DXMatrixTranslation(&Object[1].local[0], 0.0f, 5.5f, 2.5f);
	//Object[1].mtrl[0] = CHAIR_MTRL;

	//D3DXCreateBox(Device, 5.0f, 2.0f, 4.0f, &Object[1].mesh[1], 0);
	//D3DXMatrixTranslation(&Object[1].local[1], 0.0f, 3.5f, 0.0f);
	//Object[1].mtrl[1] = CHAIR_MTRL;

	//D3DXCreateBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[2], 0);
	//D3DXMatrixTranslation(&Object[1].local[2], 1.5f, 0.5f, 2.5f);
	//Object[1].mtrl[2] = CHAIR_MTRL;

	//D3DXCreateBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[3], 0);
	//D3DXMatrixTranslation(&Object[1].local[3], -1.5f, 0.5f, 2.5f);
	//Object[1].mtrl[3] = CHAIR_MTRL;

	//D3DXCreateBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[4], 0);
	//D3DXMatrixTranslation(&Object[1].local[4], -1.5f, 0.5f, -1.5f);
	//Object[1].mtrl[4] = CHAIR_MTRL;

	//D3DXCreateBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[5], 0);
	//D3DXMatrixTranslation(&Object[1].local[5], 1.5f, 0.5f, -1.5f);
	//Object[1].mtrl[5] = CHAIR_MTRL;
	////end chair

	////teapot
	//Object[2].num_parts = 1;
	//D3DXCreateTeapot(Device, &Object[2].mesh[0], 0);
	//D3DXMatrixIdentity(&Object[2].local[0]);
	//Object[2].mtrl[0] = SILVER_MTRL;
	////

	////ball
	//Object[3].num_parts = 1;
	//D3DXCreateSphere(Device, 2.0f, 20, 20, &Object[3].mesh[0], 0);
	//D3DXMatrixIdentity(&Object[3].local[0]);
	//Object[3].mtrl[0] = RED_MTRL;

	////bookshelf
	//Object[4].num_parts = 8;
	//D3DXCreateBox(Device, 15.0f, 22.0f, 0.5f, &Object[4].mesh[0], 0);
	//D3DXMatrixIdentity(&Object[4].local[0]);
	//Object[4].mtrl[0] = BOOKSHELF_MTRL;

	//D3DXCreateBox(Device, 15.0f, 0.5f, 5.0f, &Object[4].mesh[1], 0);
	//D3DXMatrixTranslation(&Object[4].local[1], 0.0f, 10.75f, -2.75f);
	//Object[4].mtrl[1] = BOOKSHELF_MTRL;

	//D3DXCreateBox(Device, 15.0f, 0.5f, 5.0f, &Object[4].mesh[2], 0);
	//D3DXMatrixTranslation(&Object[4].local[2], 0.0f, -10.75f, -2.75f);
	//Object[4].mtrl[2] = BOOKSHELF_MTRL;

	//D3DXCreateBox(Device, 0.5f, 22.0f, 5.5f, &Object[4].mesh[3], 0);
	//D3DXMatrixTranslation(&Object[4].local[3], 7.5f, 0.0f, -2.5f);
	//Object[4].mtrl[3] = BOOKSHELF_MTRL;

	//D3DXCreateBox(Device, 0.5f, 22.0f, 5.5f, &Object[4].mesh[4], 0);
	//D3DXMatrixTranslation(&Object[4].local[4], -7.5f, 0.0f, -2.5f);
	//Object[4].mtrl[4] = BOOKSHELF_MTRL;

	//D3DXCreateBox(Device, 14.5f, 0.5f, 5.0f, &Object[4].mesh[5], 0);
	//D3DXMatrixTranslation(&Object[4].local[5], 0.0f, 3.58f, -2.75f);
	//Object[4].mtrl[5] = BOOKSHELF_MTRL;

	//D3DXCreateBox(Device, 14.5f, 0.5f, 5.0f, &Object[4].mesh[6], 0);
	//D3DXMatrixTranslation(&Object[4].local[6], 0.0f, -3.58f, -2.75f);
	//Object[4].mtrl[6] = BOOKSHELF_MTRL;

	//D3DXCreateBox(Device, 1.0f, 5.0f, 4.0f, &Object[4].mesh[7], 0);
	//D3DXMatrixRotationZ(&temp, D3DX_PI * 0.15f);
	//D3DXMatrixTranslation(&Object[4].local[7], -5.6f, -1.0f, -2.75f);
	//Object[4].local[7] = temp * Object[4].local[7];
	//Object[4].mtrl[7] = RED_MTRL;
	//
	////TV & STAND
	////tv box
	//Object[5].num_parts = 11;
	//D3DXCreateBox(Device, 16.0f, 9.0f, 4.5f, &Object[5].mesh[0], 0);
	//D3DXMatrixTranslation(&Object[5].local[0], 0.0f, 2.25f, 0.0f);
	//Object[5].mtrl[0] = SILVER_MTRL;

	////tv screen
	//D3DXCreateBox(Device, 12.8f, 7.2f, 0.1f, &Object[5].mesh[1], 0);
	//D3DXMatrixTranslation(&Object[5].local[1], 0.0f, 2.25f, -2.25f);
	//Object[5].mtrl[1] = BLACK_MTRL;

	////tv stand
	//D3DXCreateBox(Device, 18.0f, 0.5f, 5.0f, &Object[5].mesh[2], 0);
	//D3DXMatrixTranslation(&Object[5].local[2], 0.0f, -2.5f, 0.0f);
	//Object[5].mtrl[2] = TVSTAND_MTRL;

	//D3DXCreateBox(Device, 18.0f, 0.5f, 5.0f, &Object[5].mesh[3], 0);
	//D3DXMatrixTranslation(&Object[5].local[3], 0.0f, -6.5f, 0.0f);
	//Object[5].mtrl[3] = TVSTAND_MTRL;

	//D3DXCreateBox(Device, 18.0f, 0.5f, 5.0f, &Object[5].mesh[4], 0);
	//D3DXMatrixTranslation(&Object[5].local[4], 0.0f, -10.5f, 0.0f);
	//Object[5].mtrl[4] = TVSTAND_MTRL;
	//

	//D3DXCreateBox(Device, 0.5f, 9.0f, 5.5f, &Object[5].mesh[5], 0);
	//D3DXMatrixTranslation(&Object[5].local[5], 9.25f, -6.5f, 0.0f);
	//Object[5].mtrl[5] = TVSTAND_MTRL;

	//D3DXCreateBox(Device, 0.5f, 9.0f, 5.5f, &Object[5].mesh[6], 0);
	//D3DXMatrixTranslation(&Object[5].local[6], -9.25f, -6.5f, 0.0f);
	//Object[5].mtrl[6] = TVSTAND_MTRL;

	////define xbox 360 video game box
	//D3DXCreateBox(Device, 6.0f, 1.5f, 4.0f, &Object[5].mesh[7], 0);
	//D3DXMatrixTranslation(&Object[5].local[7], 0.0f, -5.5f, 0.0f);
	//Object[5].mtrl[7] = WHITE_MTRL;

	////define power brick
	//D3DXCreateBox(Device, 2.0f, 1.25f, 4.0f, &Object[5].mesh[8], 0);
	//D3DXMatrixTranslation(&Object[5].local[8], 0.0f, -9.5f, 0.0f);
	//Object[5].mtrl[8] = BLACK_MTRL;

	////ring of light
	//D3DXCreateTorus(Device, 0.03f, 0.4f, 3, 10, &Object[5].mesh[9], 0);
	//D3DXMatrixScaling(&temp, 1.0f, 1.0f, 0.1f);
	//D3DXMatrixTranslation(&Object[5].local[9], 1.5f, -5.5, -2.0f);
	//Object[5].local[9] = temp * Object[5].local[9];
	//Object[5].mtrl[9] = XBOX_MTRL;

	//D3DXCreateSphere(Device, 0.1f, 15, 15, &Object[5].mesh[10], 0);
	//D3DXMatrixTranslation(&Object[5].local[10], -0.4f, -9.5f, -2.0f);
	//Object[5].mtrl[10] = XBOX_MTRL;
	//
	////cards
	//Object[6].num_parts = 5;
	//D3DXCreateBox(Device, 1.0f, 0.1f, 1.5f, &Object[6].mesh[0], 0);
	//D3DXMatrixRotationY(&temp, D3DX_PI * 0.75f);
	//D3DXMatrixTranslation(&Object[6].local[0], -5.0f, -0.5f, 5.0f);
	//Object[6].local[0] = temp * Object[6].local[0];
	//Object[6].mtrl[0] = RED_MTRL;

	//D3DXCreateBox(Device, 1.0f, 0.1f, 1.5f, &Object[6].mesh[1], 0);
	//D3DXMatrixRotationY(&temp, D3DX_PI * 0.65f);
	//D3DXMatrixTranslation(&Object[6].local[1], -5.0f, -0.5f, 7.0f);
	//Object[6].local[1] = temp * Object[6].local[1];
	//Object[6].mtrl[1] = RED_MTRL;

	//D3DXCreateBox(Device, 1.0f, 0.1f, 1.5f, &Object[6].mesh[2], 0);
	//D3DXMatrixRotationY(&temp, D3DX_PI * 0.85f);
	//D3DXMatrixTranslation(&Object[6].local[2], -5.5f, -0.5f, 3.5f);
	//Object[6].local[2] = temp * Object[6].local[2];
	//Object[6].mtrl[2] = RED_MTRL;

	//D3DXCreateBox(Device, 1.5f, 0.1f, 1.0f, &Object[6].mesh[3], 0);
	//D3DXMatrixRotationY(&temp, D3DX_PI * 0.25f);
	//D3DXMatrixTranslation(&Object[6].local[3], -3.0f, -0.5f, 5.0f);
	//Object[6].local[3] = temp * Object[6].local[3];
	//Object[6].mtrl[3] = RED_MTRL;

	//D3DXCreateBox(Device, 1.5f, 0.1f, 1.0f, &Object[6].mesh[4], 0);
	//D3DXMatrixRotationY(&temp, D3DX_PI * 0.35f);
	//D3DXMatrixTranslation(&Object[6].local[4], -7.0f, -0.5f, 3.0f);
	//Object[6].local[4] = temp * Object[6].local[4];
	//Object[6].mtrl[4] = RED_MTRL;

	////Game cases
	//Object[7].num_parts = 4;
	//D3DXCreateBox(Device, 2.5f, 0.25f, 3.75f, &Object[7].mesh[0], 0);
	//D3DXMatrixTranslation(&Object[7].local[0], 0.0f, 0.0f, 0.0f);
	//Object[7].mtrl[0] = GREEN_MTRL;

	//D3DXCreateBox(Device, 2.5f, 0.28f, 3.72f, &Object[7].mesh[1], 0);
	//D3DXMatrixTranslation(&Object[7].local[1], -0.07f, 0.0f, 0.0f);
	//Object[7].mtrl[1] = BLUE_MTRL;

	////Game cases
	////Object[7].num_parts = 2;
	//D3DXCreateBox(Device, 2.5f, 0.25f, 3.75f, &Object[7].mesh[2], 0);
	//D3DXMatrixRotationY(&temp, D3DX_PI * 0.25f);
	//D3DXMatrixTranslation(&Object[7].local[2], 1.25f, 0.25f, 0.0f);
	//Object[7].local[2] = temp * Object[7].local[2];
	//Object[7].mtrl[2] = GREEN_MTRL;

	//D3DXCreateBox(Device, 2.5f, 0.28f, 3.70f, &Object[7].mesh[3], 0);
	//D3DXMatrixTranslation(&Object[7].local[3], 1.18f, 0.25f, 0.05f);
	//Object[7].local[3] = temp * Object[7].local[3];
	//Object[7].mtrl[3] = RED_MTRL;
